#include <windows.h>  //Windows Header
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>
#include <stdio.h>

#define EPSILON 0.000001	//浮点数比较大小的精度
#define cube_l	0.05   //cube的边长 
#define Radius	1		//单位球
#define LEN		(int)(Radius/cube_l + 1)		//包围盒半条边有多少个cube
#define NUM		2*LEN+1	//包围盒一条边有多少个点
#define INOUT(distance) (Radius-distance)>EPSILON?-1:(distance-Radius)>EPSILON?1:0	
//距球心distance的点是否在球内-1-内 1-外 0-球面
static float center[3] = { 0,0,0 };//球心

FILE *fp;	//存储体素的文件

//体素结构体
typedef struct Voxel
{
	float x;
	float y;
	float z;
	float distance;
	int in_out;
}Voxel;

Voxel array[NUM][NUM][NUM];	//所有cube体素

//求两点间距离
float Distance(float a[3], float b[3])
{
	return sqrt((a[0] - b[0]) * (a[0] - b[0]) +
		(a[1] - b[1]) * (a[1] - b[1]) +
		(a[2] - b[2]) * (a[2] - b[2]));
}

//距原点距离
float Distance_0(float x, float y, float z) {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}


//线性插值，只求出球和棱的交点坐标
float Linear_Interpolation(float distance_0, float position_0, float distance_1, float position_1, float distance = 0.0f) {

	return position_0 + (position_1 - position_0) * (distance - distance_0) / (distance_1 - distance_0);

}


// 根据轴对称性将点写入文件
void writefile(float x, float y, float z, float distance, int in_out) {
	// x y z 坐标
	// distance 距球心距离
	// in_out 1-球外 -1-球内 0-球面
	fprintf(fp, "%f %f %f %f %d\n", x,y,z,distance,in_out);

	return;

}

//计算球面的点
void calculate_voxel() {
	float min = -(LEN * cube_l);	//遍历的范围
	float max = +(LEN * cube_l);
	
	fp = fopen("VolumeData.txt", "w");
	if (NULL == fp)
	{
		printf("无法打开VolumeData.txt");
		return;
	}

	float position[3] = { 0,0,0 };//点的坐标
	float distance = 0;
	int in_out = 0;

	float x = min;
	float y = min;
	float z = min;
	//计算体素
	for (int i = 0; i<NUM; i++, x += cube_l) {	//循环NUM次
		y = min;
		for (int j = 0; j < NUM; j++, y += cube_l) {
			z = min;
			for (int k = 0; k < NUM; k++, z += cube_l) {
				array[i][j][k].x = x;	//坐标
				array[i][j][k].y = y;
				array[i][j][k].z = z;
				array[i][j][k].distance = Distance_0(x, y, z);	//距球心距离
				array[i][j][k].in_out = INOUT(array[i][j][k].distance);	//球内还是球外
				writefile(x, y, z, array[i][j][k].distance, array[i][j][k].in_out);	//写入文件
			}
		}
	}


	//找到球面
	for (int i = 0; i < NUM; i++) {	//循环NUM次
		for (int j = 0; j < NUM; j++) {
			for (int k = 0; k < NUM; k++) {
				if (array[i][j][k].in_out == 0) {
					continue;
				}
				//一个点相邻的点有6个

				//z轴负方向
				if (array[i][j][k].z != min) {	
					//printf("1");
					if (array[i][j][k].in_out == -1 * array[i][j][k - 1].in_out) {	//一个内一个外
						//使用线性插值，求出在球面上的点
						float new_z = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].z, array[i][j][k - 1].distance, array[i][j][k-1].z, 1);
						// new_z = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].y, 2));
						writefile(array[i][j][k].x, array[i][j][k].y, new_z, Radius, 0);	//写入文件
					}
				}
				//z轴正方向
				if (array[i][j][k].z != max) {
					//printf("2");
					if (array[i][j][k].in_out == -1 * array[i][j][k + 1].in_out) {	//一个内一个外
						//线性插值
						float new_z = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].z, array[i][j][k + 1].distance, array[i][j][k + 1].z, 1);
						// new_z = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].y, 2));
						writefile(array[i][j][k].x, array[i][j][k].y, new_z, Radius, 0);	//写入文件
					}
				}

				//y轴负方向
				if (array[i][j][k].y != min) {
					//printf("3");
					if (array[i][j][k].in_out == -1 * array[i][j - 1][k].in_out) {	//一个内一个外
						//线性插值
						float new_y = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].y, array[i][j - 1][k].distance, array[i][j - 1][k].y, 1);
						// new_y = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].z, 2));
						writefile(array[i][j][k].x, new_y, array[i][j][k].z, Radius, 0);	//写入文件
					}
				}

				//y轴正方向
				if (array[i][j][k].y != max) {
					//printf("4");
					if (array[i][j][k].in_out == -1 * array[i][j + 1][k].in_out) {	//一个内一个外
						//线性插值
						float new_y = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].y, array[i][j + 1][k].distance, array[i][j + 1][k].y, 1);
						// new_y = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].z, 2));
						writefile(array[i][j][k].x, new_y, array[i][j][k].z, Radius, 0);	//写入文件
					}
				}

				//x轴负方向
				if (array[i][j][k].x != min) {
					//printf("5");
					if (array[i][j][k].in_out == -1 * array[i - 1][j][k].in_out) {	//一个内一个外
						//线性插值
						float new_x = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].x, array[i - 1][j][k].distance, array[i - 1][j][k].x, 1);
						//printf("%f %f %f %f %d\n", array[i][j][k].x, array[i][j][k].y, array[i][j][k].z, array[i][j][k].distance, array[i][j][k].in_out);
						//printf("%f %f %f %f %d\n", array[i - 1][j][k].x, array[i-1][j][k].y, array[i-1][j][k].z, array[i-1][j][k].distance, array[i-1][j][k].in_out);
						 //new_x = sqrt(pow(Radius, 2) - pow(array[i][j][k].z, 2) - pow(array[i][j][k].y, 2));
						writefile(new_x, array[i][j][k].y, array[i][j][k].z, Radius, 0);	//写入文件
					}
				}

				//x轴正方向
				if (array[i][j][k].x != max) {
					//printf("6");
					if (array[i][j][k].in_out == -1 * array[i + 1][j][k].in_out) {	//一个内一个外
						//线性插值
						float new_x = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].x, array[i + 1][j][k].distance, array[i + 1][j][k].x, 1);
						// new_x = sqrt(pow(Radius, 2) - pow(array[i][j][k].z, 2) - pow(array[i][j][k].y, 2));
						writefile(new_x, array[i][j][k].y, array[i][j][k].z, Radius, 0);	//写入文件
					}
				}
			}
		}
	}
	fclose(fp);

}




//使用opengl展示
void display_voxel() {
	fp = fopen("VolumeData.txt", "r");
	if (NULL == fp)
	{
		printf("无法打开VolumeData.txt");
		return;
	}
	glClear(GL_COLOR_BUFFER_BIT);//清颜色缓冲区
	glVertex2f(1, 1);
	glBegin(GL_POINTS);
	while (!feof(fp) && !ferror(fp)) {
		float x, y, z, distance;
		int in_out;
		fscanf(fp, "%f %f %f %f %d", &x, &y, &z, &distance, &in_out);	//读取
		if (in_out == 0) {	//在球面上
			
			glVertex3f(x, y, z);
		}

	}
	fclose(fp);
	glEnd();
	glFlush();
}


int main(int argc, char** argv) {
	
	calculate_voxel();


	


	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("单位球");
	glutDisplayFunc(display_voxel);
	glutMainLoop();
	int i = 0;
	scanf("%d", &i);
	scanf("%d", &i);

	return 0;
}

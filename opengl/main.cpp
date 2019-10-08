#include <windows.h>  //Windows Header
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>
#include <stdio.h>

#define EPSILON 0.000001	//�������Ƚϴ�С�ľ���
#define cube_l	0.05   //cube�ı߳� 
#define Radius	1		//��λ��
#define LEN		(int)(Radius/cube_l + 1)		//��Χ�а������ж��ٸ�cube
#define NUM		2*LEN+1	//��Χ��һ�����ж��ٸ���
#define INOUT(distance) (Radius-distance)>EPSILON?-1:(distance-Radius)>EPSILON?1:0	
//������distance�ĵ��Ƿ�������-1-�� 1-�� 0-����
static float center[3] = { 0,0,0 };//����

FILE *fp;	//�洢���ص��ļ�

//���ؽṹ��
typedef struct Voxel
{
	float x;
	float y;
	float z;
	float distance;
	int in_out;
}Voxel;

Voxel array[NUM][NUM][NUM];	//����cube����

//����������
float Distance(float a[3], float b[3])
{
	return sqrt((a[0] - b[0]) * (a[0] - b[0]) +
		(a[1] - b[1]) * (a[1] - b[1]) +
		(a[2] - b[2]) * (a[2] - b[2]));
}

//��ԭ�����
float Distance_0(float x, float y, float z) {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}


//���Բ�ֵ��ֻ��������Ľ�������
float Linear_Interpolation(float distance_0, float position_0, float distance_1, float position_1, float distance = 0.0f) {

	return position_0 + (position_1 - position_0) * (distance - distance_0) / (distance_1 - distance_0);

}


// ������Գ��Խ���д���ļ�
void writefile(float x, float y, float z, float distance, int in_out) {
	// x y z ����
	// distance �����ľ���
	// in_out 1-���� -1-���� 0-����
	fprintf(fp, "%f %f %f %f %d\n", x,y,z,distance,in_out);

	return;

}

//��������ĵ�
void calculate_voxel() {
	float min = -(LEN * cube_l);	//�����ķ�Χ
	float max = +(LEN * cube_l);
	
	fp = fopen("VolumeData.txt", "w");
	if (NULL == fp)
	{
		printf("�޷���VolumeData.txt");
		return;
	}

	float position[3] = { 0,0,0 };//�������
	float distance = 0;
	int in_out = 0;

	float x = min;
	float y = min;
	float z = min;
	//��������
	for (int i = 0; i<NUM; i++, x += cube_l) {	//ѭ��NUM��
		y = min;
		for (int j = 0; j < NUM; j++, y += cube_l) {
			z = min;
			for (int k = 0; k < NUM; k++, z += cube_l) {
				array[i][j][k].x = x;	//����
				array[i][j][k].y = y;
				array[i][j][k].z = z;
				array[i][j][k].distance = Distance_0(x, y, z);	//�����ľ���
				array[i][j][k].in_out = INOUT(array[i][j][k].distance);	//���ڻ�������
				writefile(x, y, z, array[i][j][k].distance, array[i][j][k].in_out);	//д���ļ�
			}
		}
	}


	//�ҵ�����
	for (int i = 0; i < NUM; i++) {	//ѭ��NUM��
		for (int j = 0; j < NUM; j++) {
			for (int k = 0; k < NUM; k++) {
				if (array[i][j][k].in_out == 0) {
					continue;
				}
				//һ�������ڵĵ���6��

				//z�Ḻ����
				if (array[i][j][k].z != min) {	
					//printf("1");
					if (array[i][j][k].in_out == -1 * array[i][j][k - 1].in_out) {	//һ����һ����
						//ʹ�����Բ�ֵ������������ϵĵ�
						float new_z = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].z, array[i][j][k - 1].distance, array[i][j][k-1].z, 1);
						// new_z = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].y, 2));
						writefile(array[i][j][k].x, array[i][j][k].y, new_z, Radius, 0);	//д���ļ�
					}
				}
				//z��������
				if (array[i][j][k].z != max) {
					//printf("2");
					if (array[i][j][k].in_out == -1 * array[i][j][k + 1].in_out) {	//һ����һ����
						//���Բ�ֵ
						float new_z = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].z, array[i][j][k + 1].distance, array[i][j][k + 1].z, 1);
						// new_z = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].y, 2));
						writefile(array[i][j][k].x, array[i][j][k].y, new_z, Radius, 0);	//д���ļ�
					}
				}

				//y�Ḻ����
				if (array[i][j][k].y != min) {
					//printf("3");
					if (array[i][j][k].in_out == -1 * array[i][j - 1][k].in_out) {	//һ����һ����
						//���Բ�ֵ
						float new_y = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].y, array[i][j - 1][k].distance, array[i][j - 1][k].y, 1);
						// new_y = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].z, 2));
						writefile(array[i][j][k].x, new_y, array[i][j][k].z, Radius, 0);	//д���ļ�
					}
				}

				//y��������
				if (array[i][j][k].y != max) {
					//printf("4");
					if (array[i][j][k].in_out == -1 * array[i][j + 1][k].in_out) {	//һ����һ����
						//���Բ�ֵ
						float new_y = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].y, array[i][j + 1][k].distance, array[i][j + 1][k].y, 1);
						// new_y = sqrt(pow(Radius, 2) - pow(array[i][j][k].x, 2) - pow(array[i][j][k].z, 2));
						writefile(array[i][j][k].x, new_y, array[i][j][k].z, Radius, 0);	//д���ļ�
					}
				}

				//x�Ḻ����
				if (array[i][j][k].x != min) {
					//printf("5");
					if (array[i][j][k].in_out == -1 * array[i - 1][j][k].in_out) {	//һ����һ����
						//���Բ�ֵ
						float new_x = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].x, array[i - 1][j][k].distance, array[i - 1][j][k].x, 1);
						//printf("%f %f %f %f %d\n", array[i][j][k].x, array[i][j][k].y, array[i][j][k].z, array[i][j][k].distance, array[i][j][k].in_out);
						//printf("%f %f %f %f %d\n", array[i - 1][j][k].x, array[i-1][j][k].y, array[i-1][j][k].z, array[i-1][j][k].distance, array[i-1][j][k].in_out);
						 //new_x = sqrt(pow(Radius, 2) - pow(array[i][j][k].z, 2) - pow(array[i][j][k].y, 2));
						writefile(new_x, array[i][j][k].y, array[i][j][k].z, Radius, 0);	//д���ļ�
					}
				}

				//x��������
				if (array[i][j][k].x != max) {
					//printf("6");
					if (array[i][j][k].in_out == -1 * array[i + 1][j][k].in_out) {	//һ����һ����
						//���Բ�ֵ
						float new_x = Linear_Interpolation(array[i][j][k].distance, array[i][j][k].x, array[i + 1][j][k].distance, array[i + 1][j][k].x, 1);
						// new_x = sqrt(pow(Radius, 2) - pow(array[i][j][k].z, 2) - pow(array[i][j][k].y, 2));
						writefile(new_x, array[i][j][k].y, array[i][j][k].z, Radius, 0);	//д���ļ�
					}
				}
			}
		}
	}
	fclose(fp);

}




//ʹ��openglչʾ
void display_voxel() {
	fp = fopen("VolumeData.txt", "r");
	if (NULL == fp)
	{
		printf("�޷���VolumeData.txt");
		return;
	}
	glClear(GL_COLOR_BUFFER_BIT);//����ɫ������
	glVertex2f(1, 1);
	glBegin(GL_POINTS);
	while (!feof(fp) && !ferror(fp)) {
		float x, y, z, distance;
		int in_out;
		fscanf(fp, "%f %f %f %f %d", &x, &y, &z, &distance, &in_out);	//��ȡ
		if (in_out == 0) {	//��������
			
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
	glutCreateWindow("��λ��");
	glutDisplayFunc(display_voxel);
	glutMainLoop();
	int i = 0;
	scanf("%d", &i);
	scanf("%d", &i);

	return 0;
}

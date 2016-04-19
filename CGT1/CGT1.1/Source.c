//FCG
//Aluno - Daniel Specht Silva Menezes
//Professor - Marcelo Gattass
#define _CRT_SECURE_NO_DEPRECATE
#include <gl/glut.h>
#include <stdio.h>
#include "color.h"

#pragma region Cabeçalhos

// Cabeçalhos de função
void LerRGB();
void LerIluminante();
float CalculaK();
float SomaRiemannParaCalcularRGB(float A[], float B[], float K, int tamanhoParticao, int lambdaInicio, int lambda_boxSize);
void BoxFunction();

#pragma endregion

#pragma region Variáveis

//Gamuts tratados
enum gamuts { CIE_RGB=1, CIE_XYZ, CIE_xyY, CIE_Lab, CIE_sRGB};

int gamutAtual = CIE_RGB;

// Variaveis do arquivo lambda-r_bar-g_bar-b_bar
float r_bar[1000];
int contR_bar = 0;

float g_bar[1000];
int contG_bar = 0;

float b_bar[1000];
int contB_bar = 0;

// Variaveis do arquivo Lambda-Standard Illuminant D65
// Iluminante padrão
float Iluminante[1000];
int contIluminante = 0;

// Comprimento de lambda
int Lambdas[1000];
int contLambdas = 0;

// Componentes da Luz branca
float componenteR = 0;
float componenteG = 0;
float componenteB = 0;

//Variáveis da câmera
GLfloat angle, fAspect;

#pragma endregion

#pragma region Janela e mouse

// Inicializa parâmetros de rendering
void Inicializa(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	angle = 45;
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(void)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);

	// Inicializa sistema de coordenadas de projeção
	glLoadIdentity();

	// Especifica a projeção perspectiva, field of view
	gluPerspective(angle, fAspect, 0.5, 500);

	// Especifica sistema de coordenadas do modelo
	// Todas as futuras alterações, tais como operações de escala, rotação e translação, irão afetar os modelos da cena
	glMatrixMode(GL_MODELVIEW);

	// Inicializa sistema de coordenadas do modelo
	glLoadIdentity();

	// Especifica posição do observador e do alvo
	gluLookAt(200, 200, 200, 0, 0, 0, 0, 1, 0);

	/**
	gluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz );

	eyeX, eyeY, eyeZ
	Specifies the position of the eye point.

	centerX, centerY, centerZ
	Specifies the position of the reference point.

	upX, upY, upZ
	Specifies the direction of the up vector.
	**/
}

// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	// Para previnir uma divisão por zero
	if (h == 0) h = 1;


	glViewport(0, 0, w, h);
	/**
	Define:
	(x,y) A área dentro janela, em coordenadas de tela, que OpenGL pode usar para fazer o desenho.
	(w,h) LArgura e altura da janela
	**/

	// Calcula a correção de aspecto
	fAspect = (GLfloat)w / (GLfloat)h;


	EspecificaParametrosVisualizacao();
}

// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
		if (state == GLUT_DOWN) {  // Zoom-in
			if (angle >= 10) angle -= 5;
		}
	if (button == GLUT_RIGHT_BUTTON)
		if (state == GLUT_DOWN) {  // Zoom-out
			if (angle <= 130) angle += 5;
		}
	EspecificaParametrosVisualizacao();
	glutPostRedisplay();
}

#pragma endregion

#pragma region Leitura de arquivos

// Executa a leitura do arquivo lambda-r_bar-g_bar-b_bar
void LerRGB()
{
	FILE *fp;
	char buff[255];
	int i = 0;

	fp = fopen("lambda-r_bar-g_bar-b_bar.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open input file in.list!\n");
	}

	while (fscanf(fp, "%s", buff) == 1)
	{

		switch (i)
		{
		case 0:
			Lambdas[contLambdas] = atoi(buff);
			contLambdas++;
			i++;
			break;
		case 1:
			r_bar[contR_bar] = atof(buff);
			contR_bar++;
			i++;
			break;
		case 2:
			g_bar[contG_bar] = atof(buff);
			contG_bar++;
			i++;
			break;
		case 3:
			b_bar[contB_bar] = atof(buff);
			contB_bar++;
			i = 0;
			break;
		}
	}
	fclose(fp);
}

// Executa a leitura do arquivo Lambda-Standard Illuminant D65
void LerIluminante()
{
	FILE *fp;
	char buff[255];
	int i = 0;

	fp = fopen("Lambda-Standard Illuminant D65.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open input file in.list!\n");
	}

	while (fscanf(fp, "%s", buff) == 1)
	{
		switch (i)
		{
		case 0:
			Lambdas[contLambdas] = atoi(buff);
			contLambdas++;
			i++;
			break;
		case 1:
			Iluminante[contIluminante] = atoi(buff) / 100.0;
			contIluminante++;
			i = 0;
			break;
		}
	}
	fclose(fp);
}

#pragma endregion

#pragma region Cálculo e desenho do Gamut

void calculaComponentesLuzBranca()
{
	//calcula a soma
	int cont = 0;
	float resultado = 0;
	for (cont = 0; cont < contLambdas; cont++)
	{
		resultado += Iluminante[cont] * r_bar[cont];
	}
	componenteR = ((5.0)*resultado);


	resultado = 0;
	for (cont = 0; cont < contLambdas; cont++)
	{
		resultado += Iluminante[cont] * g_bar[cont];
	}
	componenteG = ((5.0)*resultado);

	resultado = 0;
	for (cont = 0; cont < contLambdas; cont++)
	{
		resultado += Iluminante[cont] * b_bar[cont];
	}
	componenteB = ((5.0)*resultado);
}

// Define o que será desenhado na tela
void Desenha(void)
{
	// GL_COLOR_BUFFER_BIT - Buffer onde a imagem é armazenada internamente e limpar o buffer com glClear remove o desenho da janela
	glClear(GL_COLOR_BUFFER_BIT);

	printf("DESENHA\n");
	// Eixo x - Red
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	glEnd();

	// Eixo y - Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glEnd();

	// Eixo z - Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	BoxFunction();

	glutSwapBuffers();
	//Dá clear no buff em exibição e flush no outro
}

void BoxFunction()
{
	// Variável que representa a "caixa", delta lambda
	int box; //Tamanho da caixa
	int boxBegin; // Ponto de início da caixa em um determinado momento
	float K = CalculaK();

	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	//glPointSize(3);
	//glBegin(GL_POINTS);
	// Para todos os comprimentos de caixa de 5 até 400 (780-380 = 400) realizar a metodologia da função box para B=1 explicada em aula
	for (box = 5; box <= 400; box += 5)
	{
		//printf("BOX = %d \n", box);

		// Realiza a "viagem" da caixa. De 5 em 5. De 380 até 780.
		for (boxBegin = 380; boxBegin <= 780; boxBegin += 5)
		{
			float R, G, B, X, Y, Z, x, y, z, L, a, b,sR,sG,sB;
			//printf(" %d - ",boxBegin);
			R = SomaRiemannParaCalcularRGB(r_bar, Iluminante, K, 5, boxBegin, box)*K * 5;
			G = SomaRiemannParaCalcularRGB(g_bar, Iluminante, K, 5, boxBegin, box)*K * 5;
			B = SomaRiemannParaCalcularRGB(b_bar, Iluminante, K, 5, boxBegin, box)*K * 5;

			//glColor3f(R, G, B);
			//glVertex3f(100.0*R, 100.0* G, 100.0* B); 

			//Cor RGB to XYZ
			X = (float)(R*0.490 + G*0.310 + B*0.200);
			Y = (float)(R*0.177 + G*0.813 + B*0.011);
			Z = (float)(R*0.000 + G*0.010 + B*0.990);

			corCIEXYZtosRGB(X,Y,Z,&sR,&sG,&sB,4);
			glColor3f(sR, sG, sB);


			switch (gamutAtual)
			{
			case CIE_RGB:

				glVertex3f(100.0*R, 100.0* G, 100.0* B);

				break;

			case CIE_XYZ:

				X = (float)(R*0.490 + G*0.310 + B*0.200);
				Y = (float)(R*0.177 + G*0.813 + B*0.011);
				Z = (float)(R*0.000 + G*0.010 + B*0.990);

				glVertex3f(100.0*X, 100.0*Y, 100.0*Z);

				break;

			case CIE_xyY:

				x = X / (X + Y + Z);
				y = Y / (X + Y + Z);
				z = Z / (X + Y + Z);

				glVertex3f(100.0*x, 100.0*y, 100.0*z);

				break;

			case CIE_Lab:
				corCIEXYZtoLab(X, Y, Z, &L, &a, &b, 4);
				glVertex3f(L, a, b);
				break;

			case CIE_sRGB:
				glVertex3f(100.0*sR, 100.0*sG, 100.0*sB);
				break;
			default:
				printf("\n\nO numero de gamut errado\n");
			}
		}
	}
	glEnd();
}

// Função recursiva para calcular a soma de riemann dado um valor inicial de lambda em nm e 
float SomaRiemannParaCalcularRGB(float A[], float B[], float K, int tamanhoParticao, int lambdaInicio, int lambda_boxSize)
{
	float resultado = 0; // Resultado da soma.
	int NParticoes = 0; // Número de partições da soma de rinemann contempladas por esta função.
	int contParticoes; // Contador auxiliar de partições para o loop.


					   // Cada chamada desta função só pode chegar até lambda 780. Caso passe disso, a box volta ao comprimento 380.
	if ((lambdaInicio + lambda_boxSize)> 780)
	{
		NParticoes = (780 - lambdaInicio) / tamanhoParticao;
	}
	else
	{
		NParticoes = (lambda_boxSize) / tamanhoParticao;
	}

	// Realiza a soma até no máximo 780nm.
	for (contParticoes = 0; contParticoes < NParticoes; contParticoes += 1)
	{
		int index = ((lambdaInicio - 380) / 5) + contParticoes;
		resultado += A[index] * B[index];
	}

	// Caso a box ultrapasse 780nm, esta mesma função é chamada novamente do início, 380nm.
	if ((lambdaInicio + lambda_boxSize)> 780)
	{
		resultado += SomaRiemannParaCalcularRGB(A, B, K, tamanhoParticao, 380, lambda_boxSize - (780 - lambdaInicio));
	}

	return  resultado;
}

//Calcula a constante k
float CalculaK()
{
	float maxvalue[3], max;
	int n = 3, cont;
	maxvalue[0] = componenteR;
	maxvalue[1] = componenteG;
	maxvalue[2] = componenteB;

	printf("Yr = %f\nYg = %f\nYb = %f\n", componenteR, componenteG, componenteB);

	max = maxvalue[0];
	//Escolhe a componente da luz branca de maior valor para calcular K
	for (cont = 0; cont < 3; cont++)
	{
		if (maxvalue[cont] > max)
		{
			max = maxvalue[cont];
		}
	}

	printf("\nmax = %f\n", max);

	return 1 / max;
}

#pragma endregion

// Programa Principal
int main(int argc, char *argv[])
{
	int cont = 0;
	//enum gamuts { CIE_RGB, CIE_XYZ, CIE_xyY, CIE_Lab, CIE_sRGB};

	int gamutSelecionado =0 ;

	printf("Qual Gamut deve ser exibido?\n 1 - CIE_RGB\n 2 - CIE_XYZ\n 3 - CIE_xyY\n 4 - CIE_Lab\n 5 - CIE_sRGB\n");
	scanf("%d",&gamutSelecionado);

	while (!((gamutSelecionado >=1) && (gamutSelecionado <=5)))
	{
		printf("\n Valor inserido invalido. Selecione outro \n");
		printf("\n 1 - CIE_RGB\n 2 - CIE_XYZ\n 3 - CIE_xyY\n 4 - CIE_Lab\n 5 - CIE_sRGB\n");
		scanf("%d",&gamutSelecionado);
	}

	gamutAtual = gamutSelecionado;

	LerIluminante();
	LerRGB();
	calculaComponentesLuzBranca();
	printf("K = %f \n", CalculaK());
	printf("Rbranco %f \n Gbranco %f \nBbranco %f \n ", componenteR, componenteG, componenteB);

	for (cont = 0; cont < contLambdas; cont++)
	{
		printf("%d - %d - %.6f - %.6f - %.6f -%.6f\n ", cont, Lambdas[cont], r_bar[cont], g_bar[cont], b_bar[cont], (Iluminante[cont] / 100.0));
	}

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	/**
	Avisa a GLUT que tipo de modo de exibição deve ser usado
	Single-buffered (GLUT_SINGLE)
	Todos os comandos de desenho são feitos na janela de exibição
	Double-duffered (GLUT_DOUBLE)
	GLUT_DOUBLE. Isto faz com que todo o rendering seja feito em um offscreen buffer
	Modo de cores RGBA (GLUT_RGB)
	**/
	glutInitWindowSize(700, 700);
	//Determina qual o tamanho da janela
	glutInitWindowPosition(10, 10);
	//Determina a posição da janela na tela
	glutCreateWindow("GAMUT - Iluminante CIE D65");
	glutDisplayFunc(Desenha);
	//Estabelece o callback de exibição, chamada sempre que a janela precisar ser redesenhada, inclusive em casos de redimensionamento de tela
	glutReshapeFunc(AlteraTamanhoJanela);
	//Estabelece o Callback de redimensionamento, inicia/reinicia o sistema de coordenadas
	glutMouseFunc(GerenciaMouse);
	//Estabelece a função de callback para gerenciar os eventos do mouse
	Inicializa();
	//Inicializa alguns parâmetros de rendering
	glutMainLoop();
	//Inicializa a execução da “máquina de estados” até que o programa finalize
}

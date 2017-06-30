/*
   Copyright (c) 2003-2005
   Rui Ferreira, Ricardo Barreira, Nuno Cerqueira, Rui Martins
   University of Porto
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   3. Neither the name of the University of Porto nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#include "Mouse.h"

void dealwithexit(void)
{
	puts("Exiting...");
	exit(0);
}


int main(int argc, char *argv[])
{ 
	atexit(dealwithexit);
	Mouse *xip = new Mouse(argc, argv);
	xip->behave();
	return 0;
}

Mouse::Mouse(int argc, char *argv[])
{
	// faz parsing da linha de comandos
	parse_opt(argc,argv);

	// Cria uma nova ligação
	con = new Connection(host,name,pos);  

	// cria o estado do mundo
	ws = new WorldState(pos,log);

	// faz log caso seja pedido
	if (dolog) log = new Log(ws,true);
	else log = new Log(ws,false);

	ws->setLog(log);
	// cria o controlador dos motores
	motor = new MotorController(ws,log);

	// cria o controlador do alto nível
	med = new Meditator(ws,motor);

	// liga-se ao servidor
	con->connect();


	// inicializa a parte do estado do mundo que apenas pode ser
	// inicializada depois de ligado ao servidor
	ws->onlineWSCreation();

	// inicializa o visualizador
	if (wsv)
	{
		pthread_t tid;
		wscon.ws = ws;
		wscon.con = con;
		wscon.log = log;
		pthread_create(&tid, NULL, gui, (void *) &wscon);
		pthread_detach(tid);
	}
}

void Mouse::behave()
{ 
	Action * act;

	ws->update(); 
	log->init(); // inicialização do log (tem que ser depois do 1º ws->update() 
	while(1)
	{
		ws->update(); // actualização do estado do mundo
		log->update(); // actualização do log

		SensorRequest req = med->CalculateSensorRequest();
		motor->requestSensors(req);

		if (ws->isRunning())
		{
			act = med->bestAction(); // calcula a melhor acçãoo
			motor->drive(act,req); // executa a melhor acçãoo
			delete act;
		}
		if (ws->isFinished() || ws->getTime()>=ws->getParameters()->getSimTime())
		{
			getchar();
			break;
		}
	}
}

void Mouse::parse_opt(int argc,char *argv[])
{
	if (time(NULL)%2)
		puts("                ,ijfLLfji:                        :ijfLLLft,              \n\
				,fEKKKKKKKKKWEf:       ....,t     .jDKKKKKKKKKKKL;           \n\
				:LEEKKKEEDDDDDDEWWf   .;jjt;:iL.   jEEKKKKEEDDDDDEKWD:         \n\
				:EEDWKEEDDDGGGGGLGKWD  ::.,itfGD;  LEGKWKEDDDDGGGGLGEWK,        \n\
				GEfWKEEDDDGGGGLLLLGWWf  ifjffLDKG tELEWKEDDDDGGGGLLLLKWE        \n\
				.KEEWKEDDDGGGGLLLfffKWE :..,ijLDE, LEDWKEEDDGGGGGLLfffDWW:       \n\
				EEKKEEDDDGGGLLLi.ifKWEiftiiijfGKDiGKKWKEDDDGGGGLLj::fDWW.       \n\
				jKKKKEDDGGGGLLfi;fDWWKDGLfffffLDEKKKKKKEDDDGGGLLLt;tLWWL        \n\
				LKKKEDDGGGGLLLfLEWGjiiiitttttjjffGEKKKKEDDGGGLLLffDWWG         \n\
				iDKKKEDDGGGGDEKLiii;: .;ff;. :tGGLGEWKKEEDGGGGGEKWEt          \n\
				;fDKKWWWWWKLi;,;;  .GWWK: .:;jEDLGDKWWWKWWWWWEL;            \n\
				:,;;fEt;,..,:  :DWWW, .:;tGWfLGDWWWL;;,:                \n\
				.EL;;. :;:  .:;tfiGGfijGEfLGGEWWW.                   \n\
				iEL;,.:;i;  .,;tfKWWWffELfLGGEWWWi                   \n\
				.......tELii;fLLji,,;jGGGDLfjLffDDDGDKKKj.......            \n\
				.::.::.iEEttjDKKEDDDGEKWWWEGGDDDKEEGDKKKi.......            \n\
				.......:GELfjjEKKDLLLfLGGGffLLDKEDEEKKKD:.......            \n\
				,KKffffEWWKGjjjjjfffGEWWKEEKWWW;                    \n\
				;KKLtttLKWWWKEEEEKWWWWEDEWWWWi                     \n\
				:LKELjjjfGGGDDDEWWWWEEKWWWD:                      \n\
				;GKKDGLLGGGGGEWWWWWWWWG;                        \n\
				XIP 2005                 .tLKKGGGGGGKWWWWWGt:                          \n\
				http://zips.mine.nu/~xip/    ,jLGjDGLfti:                              \n");
	else
		puts("\n\
				,;;;;;.               :;;;;;;ijjj;;iijttjjt;;;;;;;;;;;;;;;;;;;.    \n\
				W######L:           :G#######GfjfE#DLEWfjjfK##################t    \n\
				W########L:       :G#########DfjfEDfjDKLjjLW##################t    \n\
				:,,,;D#####G:   :G#####G;,,,,,ifE;.;. ,GGt;,,,,,,,,,,,,,,,,,,,.    \n\
				;D#####L,G#####G:        iL;:;L:;tL      ;DDDDDDDDDf;        \n\
				;D#####EK##G:           LLGLjtjG:      t###########Wi      \n\
				iK#####Ki              ;Ljtt;        t####WGDW#####G     \n\
				:G##EK#####L:           LWWWWWWK       t####G   ;K####;    \n\
				:G#####L;D#####L:         fEE####W       t####G    L####i    \n\
				.:::,G#####D:   ;D#####L,::::::::::W###W:::::::tKKKKL:,iG####W:    \n\
				W########G:       ,D########################################K,     \n\
				W######G:           ;D####################################Kf.      \n\
				;iiiii:               ,iiiiiiiiiiiiiiiiiiiiiiiifKKKKGiii;:         \n\
				t####G              \n\
				t####G              \n\
				XIP 2005                                     t####G              \n\
				http://zits.mine.nu/~xip/                    t####G              \n");

	strcpy(host,"localhost");
	strcpy(name,"XIP");
	strcpy(pwsname,"erro");
	pos=1;
	dolog=false;
	pws=false;
	wsv=false;

	const char * short_options = "hs:n:r,p:w:vl";

	const struct option long_options[] = {
		{ "help", 0, NULL, 'h' },
		{ "host", 1, NULL, 's' },
		{ "name", 1, NULL, 'n' },
		{ "robname", 1, NULL, 'r' },
		{ "pos", 1, NULL, 'p' },
		{ "ws", 1, NULL, 'w' },
		{ "viewer", 0, NULL, 'v' },
		{ "log", 0, NULL, 'l' },
		{0}};

	int next_option;

	do {
		next_option = getopt_long_only(argc,argv, short_options,
				long_options, NULL);

		switch (next_option)
		{
			case 's':
				strcpy(host,optarg);
				break;
			case 'n':
			case 'r':
				strcpy(name,optarg);
				break;
			case 'p':
				sscanf(optarg,"%d",&pos);
				break;
			case 'w':
				pws=true;
				strcpy(pwsname,optarg);
				break;
			case 'v':
				wsv = true;
				break;
			case 'l':
				dolog=true;
				break;
			case -1:
				break;
			case 'h':
			default:
				fprintf(stderr,
						"modo de uso: xip [-s hostname[:portnumber]] [-n robotname] [-p posnumber] [-v] [-l] [-w WorldStatefile]\n\
						\t-s, -host\t\t\tPara indicar a maquina e a porta onde executa o servidor\n\
						\t-h, -help\t\t\tPara ler esta nota\n\
						\t-n, -r, -name, -robname\t\tPara especificar o nome do rato\n\
						\t-p, -pos\t\t\tPara indicar a posição inicial\n\
						\t-v, -viewer\t\t\tPara executar com o visualizador grafico\n\
						\t-l, -log\t\t\tPara guardar log das acções do rato\n\
						\t-w, -worldstate\t\t\tPara executar com um mapa gerado pelo XIP maze editor\n\
						");
				exit(EXIT_FAILURE);
		}

	} while(next_option!= -1);


};

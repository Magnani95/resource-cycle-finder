/* 	--BEGIN OF FILE  Macro to import in the beginning of file to activate debug
	funtion.
---###
#define [STABLE|NSTABLE]
#if defined STABLE && defined DEBUG
	#define STORE_DEBUG
	#undef DEBUG
#endif
#DEFINE FILE [<file-name>]
#DEFINE FUNCTION [<fun-name>]

	--END OF FILE Macro to import in the end of file. It will restore the status
	of debug macro
---###
#if defined STORE_DEBUG && !defined DEBUG
	#define DEBUG
	#undef STORE_DEBUG
#endif
---###
*/
//------------------------------------------------------------------------------
//-- Activate or not the DEBUG status, without the need to pass it to the compiler
//#define DEBUG

//--------------DISATTIVATORI
//---------------IF AND ELSE
#ifdef DEBUG
	#include<iostream>
	#include<cstdio>
	#define MESSAGE(a)		std::cout<<"\n--MSG\t"<<(a);

	#ifndef ND_GRAFICA
		#define DEB_GRAFICA	/**<Attiva le macro di debug per le componenti grafiche*/
	#endif
	#ifndef ND_VAL
		#define DEB_VAL		/**<Attiva le macro di debug per seguire il flusso del programma e stampare lo stato delle variabili*/
	#endif
#else
	#define MESSAGE(a)

	#define NDEBUG
	#undef DEB_VAL
#endif

//----------------SOTTOMACRO MAGGIORI
//-----------------------------------

#ifdef DEB_VAL	//debug per valori e stato variabili

	/*stampa in cout (a)=file, (b)=funzione, (c)=note.*/
	#define STEP(a, b, c) 	step_debug(#a, #b, #c)

	/**Stampa in cout il valore e il nome di (a); (c) per il file o note*/
	#define PRINT(a, c) 	stampa_debug(a, #a, #c)

	/**Stampa in cout l'indirizzo e il nome del puntatore (a); (c) per file o note*/
	#define PRINT_ADDR(a,c) 	stampadd_debug(a, #a, #c)
	#define PRINTF		std::printf

#else
	#define STEP(a, b, c)
	#define PRINT(a, c)
	#define PRINT_ADDR(a, c)
	#define PRINTF	//

#endif


//-----------SOTTOMACRO STRUTTURALI)
#define step_debug(a, b, c) 		std::cout<<"\n---Step\t\t"<<(a)<<"\t\t"<<(b)<<"\t"<<(c)<<std::flush;

#define stampa_debug(a, b, c)		{std::cout<<"\n"<<c<<"\t\t---Value of\t\t"<<(b)<<"\t"<<static_cast<int>(a)<<std::flush;}

#define stampadd_debug(a, b, c)	{std::cout<<"\n"<<c<<"\t\t---Address of\t\t"<<(b)<<"\t"<<(a)<<std::flush;}



#include<cassert>

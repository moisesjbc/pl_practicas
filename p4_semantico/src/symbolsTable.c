#include "symbolsTable.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

//#define DEBUG 1

/* 
If nextSymIsFirstChild is 1, next time insertSymbol is invoked, the new symbol 
is inserted as the child of top symbol.
This variable is set to 1 after inserting a Method in symTable, and it's set to 
0 when a new symbol is inserted (local data) or "GoOutOfScope()" is invoked 
(we go out of method scope).
*/
static char nextSymIsFirstChild = 0;

// Fast access to last defined method. 
struct Method* lastDefinedMethod = NULL;

// Symbols tree's root.
static Symbol* symTable = NULL;
static Symbol* mainMethodNext = NULL;
static Symbol* mainMethod = NULL;

static char change = 0;

/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

// Create a "empty and generic" symbol defining only its symType and name.
Symbol* createSymbol ( int symType, cstr  name )
{
	// Allocate memory for new symbol.
	Symbol *symbol = (Symbol *)malloc( sizeof( Symbol ) );

	// Set symbol's type and name.
	symbol->symType = symType;
	symbol->name = (char *)malloc( strlen( name )+1 );
	strcpy( symbol->name, name );

	// Initialize symbol's "firstChild" field and pointers. 
	symbol->firstChild = 0;
	symbol->prev = NULL;
	symbol->next = NULL;

	return symbol;
}

// Insert symbol in symbols table.
void insertSymbol( Symbol *symb )
{
	assert( symb != NULL );
	printf( "Insertando symbol\n");
	if( symTable == NULL ){
		// First symbol added to symbols table.
		//symTable = symb;
		//symTable->prev = NULL;	
		//lastDefinedMethod->lastSymbol = symb;
		printf( "Insertando symbol\n");
	}else{
		// Not the first symbol added to symbols table.
		if( nextSymIsFirstChild ){
			if(symb->symType == SYM_GLOBAL)
			{	
				symb->prev = ((struct Method*)(mainMethod->info))->lastSymbol;
				((struct Method*)(mainMethod->info))->lastSymbol->next = symb;
				((struct Method*)(mainMethod->info))->lastSymbol = symb;		
			}else
			{
				// This symbol is the first child of the symbols table top.
				printf( "Insertando symbol nex is first 0\n");
				printf( "Last defined apunta a %s\n", lastDefinedMethod->lastSymbol->name);			
				symb->firstChild = 1;
				printf( "Insertando symbol nex is first 1\n");
				((struct Method *)(lastDefinedMethod->lastSymbol->info))->localSymbols = symb;
				printf( "Insertando symbol nex is first 2\n");
				symb->prev = lastDefinedMethod->lastSymbol;
				printf( "Insertando symbol nex is first 3\n");
				printf( "Insertando %s (primer hijo), nodo anterior: NULL\n", symb->name );		
				lastDefinedMethod->lastSymbol = symb;	
				nextSymIsFirstChild = 0;				
			}		
		}else{
			if(symb->symType == SYM_GLOBAL)
			{	
				symb->prev = ((struct Method*)(mainMethod->info))->lastSymbol;
				((struct Method*)(mainMethod->info))->lastSymbol->next = symb;
				((struct Method*)(mainMethod->info))->lastSymbol = symb;		
			}else
			{		
				// This symbols is the brother of the symbols table top.
				printf( "Insertando no next\n");
				printf( "Insertando %s, nodo anterior: %s\n", symb->name, lastDefinedMethod->lastSymbol->name );
				symb->prev = lastDefinedMethod->lastSymbol;
				lastDefinedMethod->lastSymbol->next = symb;
				if( ((struct Method*)(mainMethod->info))->lastSymbol == lastDefinedMethod->lastSymbol) 
				{
					symTable = symb;
					((struct Method*)(mainMethod->info))->lastSymbol = symb;
				}
				lastDefinedMethod->lastSymbol = symb;
				printf( "Last defined apunta a %s\n", lastDefinedMethod->lastSymbol->name);										
			}			
			nextSymIsFirstChild = 0;
		}
	
	}
	//Update the lastSymbol of the current method
	printf( "Cambiando last define por %s\n",symTable->name); 

	setChanged();
}

// Create a variable's symbol defining its symType (SYM_VARIABLE, SYM_CONSTANT
// or SYM_GLOBAL) and its name.
Symbol* createVariable( int symType, cstr name)
{
	Symbol* variableStruct = createSymbol( symType, name );
	variableStruct->info = (void *)malloc( sizeof( struct Variable ) );
	((struct Variable *)(variableStruct->info))->type = NULL;
	return variableStruct;
}


// Search in symbols' table for variable with symType "symType" and name "name"
// and return it. If not found, create and return it.
// The "attribute" argument indicates if the variable is simple, or an array or
// class element.
Symbol* getCreateVariable( int symType, cstr name, struct SymbolInfo* atribute)
{
	Symbol* variableStruct = searchVariable( symType, name );
	if(atribute == NULL)
	{
		yyerror("Atribute should never be null");
	}
	
	if( atribute->info == 0)//No atribute info 
	{	//Variable with a simple type
		printf("atribute info es 0\n");
		if( variableStruct == NULL)
		{
			variableStruct = createVariable(symType, name);
			//variableStruct = createSymbol( symType, name );
			//variableStruct->info = (void *)malloc( sizeof( struct Variable ) );
			//((struct Variable *)(variableStruct->info))->type = NULL;
			//variableStruct->info = NULL;
		}
	}else //Array or class variable
	{
		printf("Atribute info no es 0 \n");
		if(variableStruct != NULL )
		{	//The variable already exists
			printf("LA variable existe \n");
			if(atribute->info == TYPE_ARRAY){
				//Atribute is [] so this variable must be an array
				printf("Variable no es null y atribute es de tipo array\n");
				struct Variable* variable = ((struct Variable*)(variableStruct->info));
				if( ((struct Type*)(variable->type)) != NULL ){
					if(((struct Type*)(variable->type->info))->id != TYPE_ARRAY)
					{
						yyerror("Type error: [] operator can not be applied on variable");
					}
				}	
			}	
		}else{
			variableStruct = createVariable(symType, name);
		}
	}	
	return variableStruct;
}

// Create a array type symbol for a array of size n whose elements are of type
// "type".
Symbol* createArraySymbol( Symbol* type, unsigned int n )
{
	printf( "CREANDO ARRAY DE TAM %d de tipo %s\n", n, type->name );
	char arrayName[50] = "";
	char index[5];
	sprintf( index, "%d", n );
	strcat(arrayName, "array_");
	strcat(arrayName, type->name);
	strcat(arrayName, "_");
	strcat(arrayName, index);
	Symbol* symbol = createSymbol( SYM_TYPE, arrayName );

	symbol->info = (void *)malloc( sizeof( struct Type ) );

	struct Type* arrayType = ((struct Type*)(symbol->info));
	arrayType->id = TYPE_ARRAY;
	arrayType->arrayInfo = malloc( sizeof( struct ArrayType ) );
	arrayType->arrayInfo->type = type;
	arrayType->arrayInfo->nElements = n;

	printf( "Creado simbolo array - nombre[%s] tipo [%s], nElements [%d]\n", 
		arrayName, arrayType->arrayInfo->type->name, arrayType->arrayInfo->nElements );

	return symbol;
}

/*                       2. Specific symbols insertion                        */

// Auxiliar function for inserting the two first symbols in first table:
// main and puts methods.
void insertMainPuts()
{
	printf( "Insertando main\n");
	// Create and fill the method's symbol.
	Symbol* mainSymbol = createSymbol( SYM_METHOD, "_main" );
	Symbol* putsSymbol = createSymbol( SYM_METHOD, "puts" );
	
	mainSymbol->firstChild = 1;
	putsSymbol->firstChild = 0;
	
	mainSymbol->info = (void *)malloc( sizeof( struct Method ) );
	putsSymbol->info = (void *)malloc( sizeof( struct Method ) );
	
	((struct Method *)(mainSymbol->info))->nArguments = 0;	
	((struct Method *)(putsSymbol->info))->nArguments = 1;	
	
	symTable = putsSymbol;
	mainMethodNext = putsSymbol;
	
	mainSymbol->prev = NULL;
	mainSymbol->next = NULL;
	((struct Method *)(mainSymbol->info))->localSymbols = putsSymbol;
	((struct Method *)(mainSymbol->info))->lastSymbol = putsSymbol;
	
	putsSymbol->prev = mainSymbol;
	putsSymbol->next = NULL;
	((struct Method *)(putsSymbol->info))->localSymbols = NULL;
	((struct Method *)(putsSymbol->info))->lastSymbol = putsSymbol;
	
	lastDefinedMethod = ((struct Method *)(mainSymbol->info));		
	mainMethod = mainSymbol;
	printf( "Main y puts insertados\n");
	nextSymIsFirstChild = 0;
	setChanged();
}

// Insert a block or method definition symbol in symbols' table.
void insertMethodBlockDefinition_( Symbol* symbol )
{
	printf( "Insertando block metod\n");
	symbol->info = (void *)malloc( sizeof( struct Method ) );

	if( symbol->symType == SYM_BLOCK ){
		// We only allow the block "each", which only has one mandatory 
		// argument.
		((struct Method *)(symbol->info))->nArguments = 1;
	}else{
		// This will be filled when method's argument is read.
		((struct Method *)(symbol->info))->nArguments = 0;		
	}

	((struct Method *)(symbol->info))->localSymbols = NULL;
	((struct Method *)(symbol->info))->lastSymbol = symbol;
	((struct Method *)(symbol->info))->returnType = NULL;
	
	//lastDefinedMethod->lastSymbol = symbol;
	// Insert method's symbol in table.
	insertSymbol( symbol );
	
	// When the final argument is declared, we'll use this pointer to access
	// this method and fill its nArgument field.
	lastDefinedMethod = ((struct Method *)(symbol->info));
	//lastDefinedMethod->lastSymbol = symbol;	

	//((struct Method *)(symbol->info))->lastSymbol = symbol;
	// If we don't go out of scope, next symbol will be a "child".
	nextSymIsFirstChild = 1;
}

// Insert method "name" in symbols table. If we don't go out of scope (by 
// calling "goOutOfScope()"), next symbols will be added as method's local data.
void insertMethodDefinition( cstr name  )
{
	printf( "Insertando metod\n");
	// Create and fill the method's symbol.
	Symbol* symbol = createSymbol( SYM_METHOD, name );

	// Fill and insert the method's symbol.
	insertMethodBlockDefinition_( symbol );
}

// Insert block with argument "argName" in symbols table. If we don't go out of 
// scope (by calling "goOutOfScope()"), next symbols will be added as block's 
// local data.
void insertBlockDefinition( cstr name, cstr argName  )
{
	printf( "Insertando bloque con variable [%s]\n", argName );

	// Create and fill the block's symbol.
	Symbol* symbol = createSymbol( SYM_BLOCK, name );

	// Fill and insert the block's symbol.
	insertMethodBlockDefinition_( symbol );

	// Insert the block's argument, it's type will be set later
	insertVariable( createVariable(SYM_VARIABLE, argName), NULL );
}

// Insert type with name "name" and id "typeId" in symbols table.
void insertTypeDefinition( cstr name, int typeId )
{	
	printf( "Insertando tipo %i\n", typeId );
	
	Symbol* symbol = createSymbol( SYM_TYPE, name );

	symbol->info = (void *)malloc( sizeof( struct Type ) );
	
	((struct Type *)(symbol->info))->id = typeId;

	insertSymbol( symbol );
}

// Insert variable "symbol" of type "type" in symbols table.
void insertVariable( Symbol *symbol, Symbol *type )
{	
	((struct Variable *)(symbol->info))->type = (void *)type;

	insertSymbol( symbol );
}

// Insert symbol of a array type with n elements of type "type".
void insertArray( Symbol* type, unsigned int n )
{
	Symbol* symbol;
	printf("Insert array type %s size %d\n", type->name, n);
	symbol = createArraySymbol(type, n);
	printf("Insert symbol type %s\n", symbol->name);
	insertSymbol(symbol);
	printf("Insert symbol done\n");
	showSymTable();
}

/*                             3. Symbol search                               */

// Search in symbols table for a type with id "id".
Symbol* searchType( int typeId )
{
	printf("Buscando type %d, symbtable apunta a %s\n", typeId, symTable->name);
	Symbol* s = symTable;
	while( s != NULL ){
		if( s->symType == SYM_TYPE ){
			if( ((struct Type *)(s->info))->id == typeId ){
				return s;
			}
		}

		s = s->prev;
	}

	showSymTable( symTable, 0 );

	printf( "\n\n\n\n\n NULLAZO AL CANTO (%d)\n\n\n\n\n", typeId );
	return NULL;
}

// FALTA: Hay que hacer que si estamos buscando en un bucle, no se tiene acceso
// a las variables globales.y en el método exterior.
// Search in symbols table for a variable with type "symType" and name "name".
Symbol* searchVariable( int symType, cstr name )
{
	Symbol* s;
	if(symType == SYM_GLOBAL)
	{
		s = ((struct Method*)(mainMethod->info))->lastSymbol;
	}
	else
	{
		s = lastDefinedMethod->lastSymbol;
	}
	//Symbol* previousSymbol = s;
	
	while( s != NULL){
		if( s->symType == symType && (strcmp(s->name, name) == 0)  ){
			printf( "Variable %s Encontrada \n", name );
			return s;
		}
		//If prev symbol is a method it could be the parent or the brother.
		//If it is the parent stop the search 	
		if(s->symType == SYM_METHOD && ((struct Method *)(s->info))->localSymbols == lastDefinedMethod->localSymbols)
		{
			printf( "Variable %s no encontrada caso yupi\n", name );
			return NULL;
		}	
		//previousSymbol = s;
		s = s->prev;
		
		/*if(s != NULL && s->symType == SYM_METHOD && ((struct Method *)(s->info))->localSymbols == previousSymbol->prev)
		{
			printf( "Variable %s no encontrada \n", name );
			return NULL;
		}	*/			
	}

	showSymTable( symTable, 0 );

	printf( "Variable %s no encontrada \n", name );
	return NULL;
}

// Search in symbols table for a method "name".
Symbol* searchMethod(cstr name )
{
	Symbol* s = mainMethodNext;
	printf("mainMethodNext apunta a %s\n", s->name);
	while( s != NULL ){
		printf("symtype = %d, name = %s\n", s->symType, s->name);
		if( s->symType == SYM_METHOD && (strcmp(s->name, name) == 0)  ){
			return s;
		}

		s = s->next;
	}

	showSymTable( symTable, 0 );
	printf( "Metodo %s no encontrado \n", name );
	return NULL;
}

// Search in symbols table for the n-th argument of method "method".
Symbol* searchNArgument(Symbol *method, int n)
{
	if(method == NULL || n > ((struct Method *)(method->info))->nArguments 
		|| n <= 0)
	 return NULL; 
	int i;
	Symbol* argument = ((struct Method *)(method->info))->localSymbols;
	for (i = 1; i < n; i++)
	{
		argument = argument->next;
	}
	return argument;
}

/*                       4. Symbols table management                          */

// Insert a "main method" and the basic methods (I/O) and types (integer, 
// float, etc).
void initializeSymTable()
{
	printf("Antes de main\n");
	//insertMethodDefinition( "_main" );
	insertMainPuts();
	printf("Despues de main\n");
	showSymTable();
	//lastDefinedMethod = (struct Method *)(symTable->info);
	printf("El last method apunta a %s \n",symTable->name); 
	//insertMethodDefinition( "puts" );
	//printf("Despues de puts\n");
	//En set main establecemos el primer hijo de main.
	//setMain();
	//TODO Insertar codigo para que funcione el puts
	//y ademas ponerle un argumento string
	struct Method* scope = getCurrentScope();
	//printf("----->Scope apunta a %s\n", scope->lastSymbol->name);
	insertMethodDefinition( "getc" );
	//TODO Insertar codigo para que funcione el getc
	goInScope(scope);		
	insertTypeDefinition( "integer", TYPE_INTEGER );
	insertTypeDefinition( "float", TYPE_FLOAT );
	insertTypeDefinition( "string", TYPE_STRING );
	insertTypeDefinition( "char", TYPE_CHAR );
	insertTypeDefinition( "boolean", TYPE_BOOLEAN );
}

// Auxiliar function. Show symbols from sym, and tabulate them according to 
// their level.
void showSymTable_( Symbol* sym, int level )
{
	int i = 0;

	while( (sym != NULL) ){
		// Tabulate current symbol.
		for( i=0; i<level; i++ ) printf( "\t" );

		// Show current symbol's name.
		switch( sym->symType ){
			case SYM_TYPE:
				printf( "TYPE" );
			break;
			case SYM_GLOBAL:
				printf( "GLOBAL" );	
			break;			
			case SYM_VARIABLE:
				printf( "VARIABLE" );
			break;
			case SYM_CONSTANT:
				printf( "CONSTANT" );
			break;		
			case SYM_METHOD:
				printf( "METHOD" );
			break;
			case SYM_BLOCK:
				printf( "BLOCK" );
			break;
			default:
				printf( "UKNOWN TYPE" );
			break;
		}

		printf( " - [%s]", sym->name );

		// If DEBUG is defined, show previous and next symbol's name.
		#ifdef DEBUG
		if( sym->prev ){
			printf( " - prev: [%s]", sym->prev->name );
		}else{
			printf( " - prev: [NULL]" );
		}

		if( sym->next ){
			printf( " - next: [%s]", sym->next->name );
		}else{
			printf( " - next: [NULL]" );
		}
		#endif

		// Show extra info according to the current symbol's type.
		Symbol* aux;
		struct ArrayType* arrayInfo;
		switch( sym->symType ){
			case SYM_TYPE:
				if(((struct Type*)(sym->info))->id == TYPE_ARRAY)
				{
					struct ArrayType *arrayInfo = ((struct Type*)(sym->info))->arrayInfo;
					printf(" - type:[%s] - nElements:[%d]\n", arrayInfo->type->name,arrayInfo->nElements);
				}else
				{
					printf("\n");
				}
				break;
			case SYM_VARIABLE:
			case SYM_GLOBAL:
			case SYM_CONSTANT:
				aux = ((struct Variable*)(sym->info))->type;
				printf( " - type: " );
				if( aux ){
					printf( "[%s]\n", aux->name );
				}else{
					printf( "NULL\n" );
				}
			break;
			case SYM_METHOD:
				aux = ((struct Method*)(sym->info))->returnType;
				if(aux == NULL){
					printf( " return type: [NULL]");
				}else{
					printf( " return type: [%s]", aux->name);
				}	
			case SYM_BLOCK:
				aux = ((struct Method*)(sym->info))->localSymbols;

				printf( " - nArguments: [%i]", ((struct Method*)(sym->info))->nArguments );
				if( aux ){
					printf( " - hijo: [%s]\n", aux->name );
					showSymTable_( aux, level+1 );
				}else{
					printf( " - hijo: [NULL]\n" );
				}
			break;
			default:
				printf( "\n" );
			break;
		}

		sym = sym->next;
	}
}

// Show the value of globals variables rilated to symbols' table.
void showGlobals_()
{
	if( nextSymIsFirstChild ){
		printf( "nextSymIsFirstChild = 1\n" );
	}else{
		printf( "nextSymIsFirstChild = 0\n" );
	}

	if( symTable ){
		printf( "symTable: [%s]\n", symTable->name );
	}else{
		printf( "symTable: [NULL]\n" );
	}

	if( mainMethod ){
		printf( "mainMethod: [%s]\n", mainMethod->name );
	}else{
		printf( "mainMethod: [NULL]\n" );
	}

	if( mainMethodNext ){
		printf( "mainMethodNext: [%s]\n", mainMethodNext->name );
	}else{
		printf( "mainMethodNext: [NULL]\n" );
	}
}

// Show the entire symbol tree.
void showSymTable()
{
	Symbol* sym = symTable;

	printf( "Symbols table -------------------------------\n" );

	showGlobals_();
	
	printf( "**********\n" );

	// Start showing from the beginning.
	while( sym->prev ){
		sym = sym->prev;
	}

	if( symTable ){
		showSymTable_( sym, 0 );
	}

	printf( "---------------------------------------------\n" );
}

// Free the memory allocated for a symbol.
void freeSymbol(Symbol* symbol)
{
	#ifdef DEBUG
	printf( "Eliminando simbolo: [%s]\n", symbol->name );
	#endif
	if(symbol->symType == SYM_TYPE)
	{
		struct Type *type = ((struct Type*)(symbol->info));
		if ( type->id == TYPE_ARRAY )
		{
			free(type->arrayInfo);
		}	
	}	
	free(symbol->name);
	free(symbol->info);
	free(symbol);
}

// Auxiliar recursive function for freeing the symbols' tree.
void freeSymbTable_( Symbol* symTable_ ){
	Symbol *aux = symTable_;
	while( aux ){
		symTable_ = aux->next;
		if( aux->symType == SYM_METHOD ){
			freeSymbTable_( ((struct Method *)(aux->info))->localSymbols );
		}		
		freeSymbol( aux );
		aux = symTable_;
	}
}

// Free the entire symbol tree.
void freeSymbTable(){
	Symbol *sym = symTable;

	if( !sym ) return;

	// Go to the beginning before start freeing.
	while( sym->prev ){
		sym = sym->prev;
	}

	freeSymbTable_( sym );
}


/*                          5. Global "change" management                     */

// Set "change" global to 1.
void setChanged()
{
	change = 1;
}

// Set "change" global to 0.
void resetChange()
{
	change = 0;
}

// Get the "change" global value.
const char getChange()
{
	return change;
}


/*                        6. Last defined method management                   */

// Return a pointer to the last defined method / block.
struct Method* getCurrentScope()
{
	return lastDefinedMethod;
}

// Set method as the last defined method.
void goInScope(struct Method *method)
{	
	nextSymIsFirstChild = 0;	
	lastDefinedMethod = method;	
}
	
// Set the last defined method's number of arguments to n. 
void setNArguments( int n ){
	assert( lastDefinedMethod );
	lastDefinedMethod->nArguments = n;
}


/*                                  7. Others                                 */

// Return the type of the elements of array type "variable". If unknown, return
// NULL.
Symbol* getArrayType(Symbol* variable)
{
	Symbol* array = ((struct Variable*)(variable->info))->type;
	if( array == NULL){
		return NULL;
	}else{
		if(array->info == NULL){
			return NULL;
		}else{	
			return ((struct Type*)(array->info))->arrayInfo->type;
		}
	}		
}

Symbol* getVariableType(int symType, cstr name, struct SymbolInfo* symbolInfo)
{
	Symbol* variable = getCreateVariable( symType, name, symbolInfo ); 
	Symbol* type = NULL;
	if(variable == NULL){
		type = NULL;
	}else{
		if( symbolInfo->info == TYPE_ARRAY ){
			type = getArrayType(variable);
		}else{
			type = ((struct Variable*)(variable->info))->type;
		}	
	}	
	free(symbolInfo);
	//This is only call on right side of expression so insert variable here
	//if it does not exits on symbol table
	printf("Antes de insertar\n");
	if(searchVariable(symType, name) == NULL){
		insertVariable(variable, NULL);
	}
	return type;
}

/*void setMain()
{
	mainMethodNext = symTable;	
	((struct Method*)(mainMethodNext->info))->lastSymbol = symTable;	 
}*/


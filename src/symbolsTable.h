#ifndef SYMBOLS_TABLE_H
#define SYMBOLS_TABLE_H

#include <stdlib.h>
#include <string.h>

extern int compilationState; 

// Un-comment this line to allow debug messages.
//#define DEBUG 1
#ifdef DEBUG
	#define DEBUG_MSG(msg,var) printf(msg,var);
#else
	#define DEBUG_MSG(msg,var)
#endif

// Macros for find out in which compilation state we are.
// FST (0) -> creating and Filling Symbol Table
// AN (1) -> doing code ANalisis
// GC (2) -> Generating Code
// NGC -> Not Generating Code
#define FST if( compilationState == 0 ){
#define EFST }

#define AN if( compilationState == 1 ){
#define EAN }

#define GC if( compilationState == 2 ){
#define EGC } ;

#define NGC if( compilationState != 2 ){
#define ENGC }


/*                                   Constants                                */
/******************************************************************************/

// Values for in Symbol symType
#define SYM_TYPE 1
#define SYM_VARIABLE 2
#define SYM_CONSTANT 3
#define SYM_GLOBAL 4
#define SYM_CLASS_VARIABLE 5
#define SYM_METHOD 6
#define SYM_BLOCK 7
#define SYM_EXTRA_INFO 8

// Values for in Variable symSubtype
#define SYM_LOCAL 1
#define SYM_ARG 2

// Values for id in Type
#define TYPE_INTEGER 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 3
#define TYPE_STRING 4
#define TYPE_BOOLEAN 5
#define TYPE_CLASS 6
#define TYPE_ARRAY 7

// Values for assignmentType in ExtraInfo
#define LOAD_ADDRESS 9

/*                                Data structs                                */
/******************************************************************************/

// Generic Symbol
struct Symbol
{
   	int symType; // symType = TYPE, VARIABLE, etc.
   	
	char *name;
	
	void *info;

	// If firstChild = 0, prev points to the previous Symbol.
	// If firstChild = 1, prev points to the parent.
	char firstChild;
	struct Symbol *prev, *next;
};	
typedef struct Symbol Symbol; 


// Extra info if symbol refers to a type.
struct Type {
	int id;
	unsigned int size;
	union 
	{
		struct ArrayType* arrayInfo; // Used if id == TYPE_ARRAY;
		struct ClassType* classInfo;   // If id == TYPE_ARRAY, this points to first element.
   	};
};
typedef struct Type Type;


// Extra info if type symbol refers to a array type.
struct ArrayType {
	Symbol* type;
	unsigned int nElements;
};
typedef struct ArrayType ArrayType;

// Extra info if type symbol refers to a class type.
struct ClassType{
	unsigned int nElements;
	Symbol **elements;
};
typedef struct ClassType ClassType;

// Extra info if symbol refers to a variable.
struct Variable {
   	Symbol* type;
   	int symSubtype; // SUBTYPE_LOCAL, SUBTYPE_ARG
	int address;
};
typedef struct Variable Variable;

// Extra info if symbol refers to a method.
struct Method {
	int nArguments;
	Symbol *lastSymbol;
	Symbol *localSymbols;
	Symbol *returnType;

	int argumentsSize, localsSize;

	int label;
};
typedef struct Method Method;

// Info related to a expression.
struct ExtraInfo {
	int assignmentType;
	int nRegister;
	Symbol* variable; 
};
typedef struct ExtraInfo ExtraInfo;

typedef const char* const cstr;

// Auxiliar struct.
struct SymbolInfo
{
	Symbol *symbol;
	Symbol *varSymbol;	
	Symbol *exprSymbol;
	int info;
	char *name;
	int nRegister;
};
typedef struct SymbolInfo SymbolInfo;

/*                                  Functions                                 */
/******************************************************************************/

/*                       1. Generic Symbols insertion                         */

// Create a "empty and generic" symbol defining only its symType and name.
Symbol* createSymbol ( int symType, cstr name );
Symbol* createExtraInfoSymbol ( int nRegister, int isFloat);

// Insert symbol in symbols table.
void insertSymbol( Symbol *symb );

// Create a variable's symbol defining its symType (SYM_VARIABLE, SYM_CONSTANT
// or SYM_GLOBAL) and its name.
Symbol* createVariable( int symType, cstr name);

// Search in symbols' table for variable with symType "symType" and name "name"
// and return it. If not found, create and return it.
// The "attribute" argument indicates if the variable is simple, or an array or
// class element.
Symbol* getCreateVariable( int symType, cstr name, SymbolInfo* attribute);

// Create a array type symbol for a array of size n whose elements are of type
// "type". 
Symbol* createArraySymbol( Symbol* type, unsigned int n );

Symbol* createClassSymbol( const char* const name );

/*                       2. Specific symbols insertion                        */

// Insert method "name" in symbols table. If we don't go out of scope (by 
// calling "goOutOfScope()"), next symbols will be added as method's local data.
void insertMethodDefinition( cstr name  );

// Insert block with argument "argName" in symbols table. If we don't go out of 
// scope (by calling "goOutOfScope()"), next symbols will be added as block's 
// local data.
void insertBlockDefinition( cstr name, cstr argName );

// Insert type with name "name" and id "typeId" in symbols table.
void insertTypeDefinition( cstr name, int typeId, int size );

// Insert variable "symbol" of type "type" in symbols table.
void insertVariable( Symbol *symbol, Symbol *type );

// Insert symbol of a array type with n elements of type "type".
void insertArray( Symbol* type, unsigned int n );


/*                             3. Symbol search                               */

// Search in symbols table for a type with id "id".
Symbol* searchType( int id );

// Search in symbols table for a variable with type "symType" and name "name".
Symbol* searchVariable( int symType, cstr name );

// Search in symbols table for a method "name".
Symbol* searchTopLevel(int symType, const char* const name );

// Search in symbols table for the n-th argument of method "method".
Symbol* searchNArgument(Symbol *method, int n);

//Searchs for the next Global Variable record in the current global variable registration
//without knowing the name
Symbol* nextGlobalVariablePointer();



/*                           4. Symbols table management                      */

// Insert a "main method" and the basic methods (I/O) and types (integer, 
// float, etc).
void initializeSymTable();

// Show the entire symbol tree.
void showSymTable();

// Free the memory allocated for a symbol.
void freeSymbol(Symbol*);

// Free the entire symbol tree.
void freeSymbTable();

// Free the memory allocated for a symbol info.
void freeSymbolInfo(SymbolInfo* symbol);


/*                         5. Global "change" management                      */

// Set "change" global to 1.
void setChanged();

// Set "change" global to 0.
void resetChange();

// Get the "change" global value.
const char getChange();


/*                        6. Last defined method management                   */

// Return a pointer to the last defined method / block.
Method* getCurrentScope();

// Set method as the last defined method
void goInScope( Method *method );

// Get the parent scope in actual position
Method *getParentScope();

// Set the last defined method's number of arguments to n. 
void setNArguments( int n );


/*                                  7. Others                                 */

// Generate a name for block whose name is "name" and argument is "argName".
char *createBlockName(cstr name, cstr argName);

// Return the type of the elements of array type "variable". If unknown, return
// NULL.
Symbol* getArrayType(Symbol* variable);  

Symbol* getVariableType(int symType, cstr name, SymbolInfo* symbolInfo );

Symbol* getClassVar( Symbol* variable, const char* const atributeName);

Symbol* getReturnType( Symbol* method);

void createPutsGetcExitCode();

// Get method's iArgument-th argument.
Symbol* getMethodArgument( Symbol* method, int iArgument );

// Iterate over methods in symbol's table and fill its fields for arguments and
// local data sizes.
void fillMethodDataSizes();

// Iterate over main Method in symbol's table and fill its fields for arguments and
// local data sizes.
void fillMainMethodDataSize();

// Return the variable height according to the block it was defined in
int returnVariableHeight( int symType, cstr name);

//void setMain();

#endif
// SYMBOLS_TABLE_H

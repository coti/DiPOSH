%option yylineno

%{
    /* * * * * * * * * * * *
     * * * DEFINITIONS * * *
     * * * * * * * * * * * */
%}

%{

    typedef struct list_t {
        char* statement;
        struct list_t* next;
    } list_t;

    list_t* declStatementsList;
    list_t* initStatementsList;
    list_t* freeStatementsList;

    typedef enum { true = 0, false = 1 } bool;

    typedef struct {
        char* name;
        char* fullName;
        char* datatype;
        char* initValue;
        int   tabDims;
        int*  tabSize;
        int   nbPtr;
    } variable_t;

    variable_t var;

    int curly = 0;
    int mFunc = 0;

    void parseText( char* );
    void parseName( char* );
    void parseInitValue( char* );
    void endOfLine( void );
    void buildStatement( void );
    void freeVarStructure( void );
    void initVarStructure( void );
    void dumpInit( void );
    void dumpFree( void );

    list_t* getNextStatement( list_t**, list_t* );
    list_t* getLastStatement( list_t* );
    list_t* insertStatementBeginning( list_t** );
    void printAllStatements( list_t* );
    void freeStatements( list_t** );
    char* concatenate( char*, char* );

    void freeList( list_t* ); // ??

%}

%s TEXTINIT
%s TEXTNONINIT
%s LOCAL
%s VALUE
%s FINISHED
%s IN_COMMENT
%s IN_FUNCTION

%%

%{
    /* * * * * * * * * 
     * * * RULES * * *
     * * * * * * * * */
%}

\{			                                   { curly++; printf( "{" ); BEGIN LOCAL; }
\}			                                   { curly--; printf( "}" ); if( 0 == curly ) { BEGIN INITIAL; mFunc = 0 ; } }

<INITIAL>^[[:space:]]*static[[:space:]].+=     { parseText( yytext ); BEGIN TEXTINIT; }
<INITIAL>^[[:space:]]*static[[:space:]][^=]+;  { parseText( yytext ); BEGIN TEXTNONINIT; }

<TEXTINIT>.+[[:space:]]*=                      { parseName( yytext ); BEGIN VALUE; }
<TEXTNONINIT>.+[[:space:]]*;                   { parseName( yytext ); BEGIN INITIAL; }

<VALUE>.+;                                     { parseInitValue( yytext ); BEGIN INITIAL; }

<INITIAL>[\n]  				                   { endOfLine(); }
<LOCAL>.*start_pes[[:space:]]*[(].*$           { printf( "%s\n", yytext ); dumpInit(); mFunc = 1; BEGIN IN_FUNCTION; }

<LOCAL>[[:space:]]*return[[:space:]]*.*$       { if( mFunc == 1 ) dumpFree(); printf( "%s\n", yytext ); }

<INITIAL>"/*"                                  { BEGIN IN_COMMENT; }
<IN_COMMENT>"*/"                               { BEGIN INITIAL; }
<IN_COMMENT>[^*\n]+                            
<IN_COMMENT>"*"                                
<IN_COMMENT>\n                                 { yylineno++; }
     
%%

 /* * * * * * * * * * * 
  * * * USER CODE * * *
  * * * * * * * * * * *
  */

 /* * * * * * * * * * * 
  * Komenkonfé :
  * - au debut, etat INITIAL
  * - on tombe sur un mot-clé static, 
  *   on appelle parseText, on recule pour parser à nouveau la ligne,
  *   on passe dans l'etat TEXT
  * - si on trouve un '=' : ce qui suit est l'initialisation : on passe dans l'etat VALUE
  *   ce qui precede est le nom
  * - si on ne trouve pas de '=' : ce qui precede le ';' est le nom
  * - entre le static et le nom, c'est le datatype
  * - on repasse dans l'etat INITIAL
  * Les accolades : on les compte, quand c'est > 0 c'est qu'on est dans un bloc
  */

void parseName( char* word ) {
    int i, startName, brackets;
    char* name;
    char* datatype;
    char* fullName;

    /* trim the ';' or '=' */
    int len = strlen( word );
    word[len-1] = '\0';
    /* trim the final whitespaces */
    i = len - 2;
    while( ' ' == word[i] ) {
        word[i] = '\0';
        i--;
    }

    /* split the name and the datatype */

    var.nbPtr = 0;    
    startName = 0;
    for( i = 1 ; i < len-1 ; i++ ) {
        if( ' ' == word[i] ) {
            if( ( i < len - 2 ) && ( ( '*' == word[ i + 1 ] ) || ( '*' == word[ i - 1 ] ) ) ) {
                startName = i;
            } else {

                startName = i + 1;
            }
        }
        if( '*' == word[i] ) {
            var.nbPtr++;
        }
    }


    /* Make sure the '*' is on the datatype, rather than on the name */

    if( var.nbPtr > 0 ) {
        for( i = 0 ; i < len-1 ; i++ ) {
            if( '*' == word[i] ) {
                word[i] = ' ';
                startName++;
            }
        }
    }

    /* If there are some spaces left at the beginning of the name, remove them */

    for( i = startName ; i < len-1 ; i++ ) {
        if( ' ' == word[i] ) {
            startName++;
        }
    }

    /* is it a statically-allocated table? */

    brackets = 0;
    for( i = startName ; i < len - 1 ; i++ ) {
        if( '[' == word[i] ) {
            if( 0 == brackets ) {
                brackets = len - i;
            }
            var.tabDims++;
        }
    }

    /* yes -- get its dimensions */

    if( 0 != brackets ) {
        char number[256];
        int k, n;
        var.tabSize = (int*) malloc( var.tabDims * sizeof( int ) );
        n = 0;

        i =  len - 1 - brackets;
        while( i < len - 1 ) {
            if( '[' == word[i] ) {
                i++;
                k = 0;
                while( ']' != word[i] ) {
                    number[k] = word[i];
                    k++;
                    i++;
                    if( i > len - 1 ) {
                        fprintf( stderr, "Error with %s: wrong syntax for a statically-allocated table, probably missing closing bracket (])\n", word );
                    }
                }
                number[k] = '\0';
                var.tabSize[n] = atoi( number );
                n++;
            }
            i++;
        }
    }


    name = (char*) malloc( len - brackets - startName + 1 );
    memcpy( (void*) name, (void*) ( word + startName ), len - startName - brackets );
    name[len - startName - brackets] = '\0';

    fullName = (char*) malloc( len - startName + 1 );
    memcpy( (void*) fullName, (void*) ( word + startName ), len - startName );
    fullName[len - startName] = '\0';

    if( var.nbPtr > 0 ) {
        datatype = (char*) malloc( startName + var.nbPtr + 1 );
        memcpy( (void*) datatype, (void*) ( word + 1 ), startName-1 );
        for( i = 0 ; i < var.nbPtr ; i++ ) {
            datatype[startName + i - 1] = '*';
        }
        datatype[startName + var.nbPtr - 1] = '\0';

    } else {
        datatype = (char*) malloc( startName );
        memcpy( (void*) datatype, (void*) ( word+1 ), startName-1 );
        datatype[startName-1] = '\0';
    }

    var.name = name;
    var.fullName = fullName;
    var.datatype = datatype;
    var.initValue = NULL;

}

void parseInitValue( char* word ) {
    /* trim the final ';' */
    int len = strlen( word );
    word[len-1] = '\0';
    var.initValue = word;
}

void parseText( char* word ) {
    int i = 0;
    while( ' ' == word[i] || '\t' == word[i] ) {
        i++;
    }
    i += strlen( "static" );

    yyless( i );
}

void endOfLine( ){
    if( NULL == var.name ) {
        printf( "\n" ); 
        return;
    }

    buildStatement();
}

list_t* getNextStatement( list_t** lst, list_t* thisStmt ){
    list_t* st = (list_t*) malloc( sizeof( list_t ) );
    if( NULL == thisStmt ) {
        st->next = NULL;
    } else {
        if( thisStmt != NULL ) {
            st->next = thisStmt->next;
        } else {
            st->next = NULL;
        }
    }
    st->statement = NULL;
    if( NULL == *lst ) {
        *lst = st;
     } else {
        if( thisStmt != NULL ) {
            thisStmt->next = st;
        }
    }
    return st;
}

list_t* getPreviousStatement( list_t** lst, list_t* st ){
    if( lst == NULL ) return NULL;
    if( *lst == st ) return st;
    
    list_t* cur = *lst;;

    while( NULL != cur ){
        if( cur->next == st ) return cur;
        cur = cur->next;
    }
    return NULL;
}

list_t* insertStatementHere( list_t** lst, list_t* putItThere ){

   if( *lst == putItThere || NULL == lst ) {
        return insertStatementBeginning( lst );        
    }

    list_t* st = (list_t*) malloc( sizeof( list_t ) );
    list_t* tmp;

    if( NULL == putItThere ) {
        *lst = st;
        st->next = *lst;
        return st ;
    } 

    printf( "insert\n" );

    /*
    tmp = putItThere->next;
    putItThere->next = st;
    st->next = tmp;
    */

    tmp = getPreviousStatement( lst, putItThere );
    if( NULL == tmp ) {
        return insertStatementBeginning( lst );        
    } else {
        tmp->next = st;
        st->next = putItThere;
    }
    
    return st;
}

list_t* insertStatementBeginning( list_t** lst ) {
    list_t* st = (list_t*) malloc( sizeof( list_t ) );
    st->next = *lst;
    st->statement = NULL;
    *lst = st;
    return st;
}

list_t* getLastStatement( list_t* lst ){
    list_t* curSt = lst;
    if( NULL == lst ) {
        return NULL;
    }
    while( NULL != curSt->next ) {
        curSt = curSt->next;
    }
    return curSt;
}

void printAllStatements( list_t* lst ){
    list_t* curSt = lst;
    while( NULL != curSt ) {
        printf( "%s\n", curSt->statement );
        curSt = curSt->next;
    }
}

void freeStatements( list_t** lst ){
    list_t* curSt = *lst;
    while( NULL != *lst ) {
        free( (*lst)->statement );
        curSt = *lst;
        *lst = curSt->next;
        free( curSt );
    }
}

char* concatenate( char* str1, char* str2 ) {
    int len = strlen( str1 ) + strlen( str2 );
    char* str = (char* ) malloc( len + 1 );
    sprintf( str, "%s%s", str1, str2 );
    return str;
}

void buildStatement( ){
    int i;

    /*fprintf( stderr, "\t\tBUILD STATEMENT\n" );
    fprintf( stderr, "\tNAME: %s\n", var.name );
    fprintf( stderr, "\tDATATYPE: %s\n", var.datatype );
    if( NULL != var.initValue ) {
        fprintf( stderr, "\tINIT: %s\n", var.initValue );
    }
    if( 0 != var.tabDims ) {
        printf( "\tTable with %d dimensions:\n", var.tabDims );
        for( i = 0 ; i < var.tabDims ; i++ ) {
            printf( "\t\tDim %d: %d\n", i,  var.tabSize[i] );
        }

    }
    fprintf( stderr, "\n" );*/

    /* serious business starts here */

    char* varName;
    int j, l;
    list_t* thisStmt;
    list_t* freeStmt;
    char* s1;

    thisStmt = freeStmt = NULL;

    if( ( 0 == var.nbPtr ) && ( 0 == var.tabDims ) ) {
        thisStmt = getNextStatement( &declStatementsList, getLastStatement( declStatementsList ) );
        asprintf( &varName, "__SHMEM__%s", var.name );
        asprintf( &(thisStmt->statement), "#define %s *%s", var.name, varName );

    } else {
        asprintf( &varName, "%s", var.name );
    }

    thisStmt = getNextStatement( &declStatementsList, getLastStatement( declStatementsList ) );

    asprintf( &s1, "static %s", var.datatype );
    l = strlen( s1 );
    free( s1 );

    if( ( 0 == var.nbPtr ) && ( 0 == var.tabDims ) ) {
        asprintf( &s1, "* %s;", varName);
        l += strlen( s1 );
        free( s1 );
    } else {
        l += var.tabDims;
        asprintf( &s1, "%s;", varName);
        l += strlen( s1 );
        free( s1 );
    }
        
    thisStmt->statement = malloc( l + 1 );
    l = 0;
    sprintf( thisStmt->statement, "static %s", var.datatype );
    asprintf( &s1, "static %s", var.datatype );
    l = strlen( s1 );
    free( s1 );

    if( ( 0 == var.nbPtr ) && ( 0 == var.tabDims ) ) {
        sprintf( thisStmt->statement + l, "* %s;", varName);
    } else {
        for( i = 0 ; i < var.tabDims ; i++ ) {
            thisStmt->statement[l] = '*';
            l++;
        }
        sprintf( thisStmt->statement + l, "%s;", varName);
    }

    thisStmt = getNextStatement( &initStatementsList, getLastStatement( initStatementsList ) );

    if( 0 == var.tabDims ) {
        asprintf( &(thisStmt->statement), "%s = (%s*) shmalloc( sizeof( %s ) );\n", varName, var.datatype, var.datatype );

        freeStmt = getNextStatement( &freeStatementsList,  getLastStatement( freeStatementsList ) );
        asprintf( &(freeStmt->statement), "shfree( %s );", varName );
    } else {

        char* ptrName;
        list_t* posFree = freeStatementsList;
        list_t* posBeforeFree = posFree;

        asprintf( &ptrName, "%s", varName );
             
        for( j = 0 ; j < var.tabDims ; j++ ) {
            int k, k2;

            bool nested;

            if( ( j > 0 ) && ( j < ( var.tabDims )  ) ) {
                nested = true;
            } else {
                nested = false;
            }

            if( true == nested ) {
                thisStmt = getNextStatement( &initStatementsList, getLastStatement( initStatementsList ) );
                asprintf( &(thisStmt->statement), "int __SHMEM__idx_%d ; \nfor( __SHMEM__idx_%d = 0 ; __SHMEM__idx_%d < %d ; __SHMEM__idx_%d++ ) { ", j-1, j-1, j-1, var.tabSize[j-1], j-1 );

                freeStmt = insertStatementHere( &freeStatementsList, posFree );

                asprintf( &(freeStmt->statement), "for( __SHMEM__idx_%d = 0 ; __SHMEM__idx_%d < %d ; __SHMEM__idx_%d++ ) { ", j-1, j-1, var.tabSize[j-1], j-1 );
            }

            /* count the length of the line */

            if( true == nested ) {
                thisStmt = getNextStatement( &initStatementsList, getLastStatement( initStatementsList ) );
                char* tmpStr;
                char* tmpPtrName;
                asprintf( &tmpStr, "[__SHMEM__idx_%d]", j-1 );
                tmpPtrName = concatenate( ptrName, tmpStr );
                free( ptrName );
                ptrName = tmpPtrName;
                free( tmpStr );

                asprintf( &s1, "%s = (%s", ptrName, var.datatype );
            }
            
            asprintf( &s1, "%s = (%s", ptrName, var.datatype );

            l = strlen( s1 );
            k = l;
            free( s1 );

            asprintf( &s1, ") shmalloc( %d * sizeof( %s ", var.tabSize[j], var.datatype, var.datatype );
            l += strlen( s1 );
            k2 = l;
            free( s1 );

            l += 2 * ( var.tabDims - j ) + 1; /* add the stars */
            k2 += var.tabDims - j ;

            /* build the statement itself */
            
            thisStmt->statement = (char*) malloc( ( l + 2 ) * sizeof( char ));
            
            sprintf( thisStmt->statement, "%s = (%s", ptrName, var.datatype );

            for( i = j+1 ; i < var.tabDims+1 ; i++ ) { /* stars of the pointer type */
                thisStmt->statement[k] = '*';
                k++;
            }
            sprintf( ( thisStmt->statement) + k, ") shmalloc( %d * sizeof( %s ", var.tabSize[j], var.datatype );
 
            for( i = j+1 ; i < var.tabDims ; i++ ) { /* stars of the datatype */
                thisStmt->statement[k2] = '*';
                k2++;
            }
            
            sprintf( ( thisStmt->statement) + k2, ") );" );

            posBeforeFree = freeStmt;
            freeStmt = getNextStatement( &freeStatementsList, freeStmt );
            if( j == 0 ) {
                asprintf( &(freeStmt->statement), "shfree( %s );", ptrName );
            } else {
                asprintf( &(freeStmt->statement), "shfree( %s ); \n}", ptrName );
            }
            posFree = freeStmt;
       }
     

        free( ptrName );
    }

    /* Close the curly brackets */
    
    for( j = 1 ; j < var.tabDims ; j++ ) {
        thisStmt = getNextStatement( &initStatementsList, getLastStatement( initStatementsList ) );
        asprintf( &(thisStmt->statement), "}");

        /*    freeStmt = getNextStatement( &freeStatementsList, freeStmt );
              asprintf( &(freeStmt->statement), "}" );*/
   }

    printAllStatements( declStatementsList );

    /* init */

    if( NULL != var.initValue ) {
        thisStmt = getNextStatement( &initStatementsList, getLastStatement( initStatementsList ) );
        asprintf( &(thisStmt->statement), "%s __SHMEM__%s = %s;", var.datatype, var.fullName, var.initValue );
        thisStmt = getNextStatement( &initStatementsList, getLastStatement( initStatementsList ) );
        asprintf( &(thisStmt->statement), "memcpy( %s, __SHMEM__%s, sizeof( __SHMEM__%s ) );", var.name, var.name, var.name );
    }

    /* The end */

    free( varName );

    freeVarStructure( );
    freeStatements( &declStatementsList ); 
}

void freeVarStructure(){

    free( var.name );
    if( NULL != var.fullName ) free( var.fullName );
    free( var.datatype );
    if( NULL != var.initValue ) { /* /!\ BIG MEMORY LEAK HERE /!\ */
        // free( var.initValue );
    }
    if( NULL != var.tabSize ) free( var.tabSize );
    initVarStructure();
}

void initVarStructure(){
    var.name = NULL;
    var.fullName = NULL;
    var.datatype = NULL;
    var.initValue = NULL;
    var.tabDims = 0;
    var.tabSize = NULL;
    var.nbPtr = 0;
}

void freeList( list_t* lst ){
    list_t* myStatement;
    while( NULL != lst ){
        free( lst->statement );
        myStatement = lst->next;
        free( lst );
        lst = myStatement;
    }
}

/* Called just after the initialization of the system: start_pes() */

void dumpInit(){
    printAllStatements( initStatementsList );
    freeStatements( &initStatementsList ); 
}

/* Called just after each 'return' keyword in the function 
   where we have found start_pes() */

void dumpFree(){
    printf( "\n" );
    printAllStatements( freeStatementsList );
    freeStatements( &freeStatementsList ); 
}

int main(int argc, char** argv) {
    int i;
    FILE* file;

    BEGIN INITIAL;

    initVarStructure();
    declStatementsList = NULL;
    initStatementsList = NULL;
    freeStatementsList = NULL;

    for( i = 1 ; i < argc ; i++ ) {
        file = fopen( argv[i], "r" );
    	if( !file ) {
            fprintf( stderr, "Could not open %s\n", argv[i] );
        }
        yyin = file;
        printf( "#include <string.h>\n" );
        
        yylex();
        
        fclose( file );
        if( argc > 2 ) {
            yywrap();
        }
    }
    
    return 0;
}

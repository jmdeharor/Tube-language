#header
<<
#include <string>
#include <iostream>
using namespace std;

// struct to store information about tokens
typedef struct {
    string kind;
    string text;
} Attrib;

// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr, int ttype, char *textt);
>>

<<
#include <cstdlib>
#include <cmath>
// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
    attr->kind = text;
    attr->text = "";
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
    AST* as = new AST;
    as->kind = attr->kind;
    as->text = attr->text;
    as->right = NULL;
    as->down = NULL;
    return as;
}

/// create a new "list" AST node with one element
AST* createASTlist(AST *child) {
    AST *as=new AST;
    as->kind="list";
    as->right=NULL;
    as->down=child;
    return as;
}

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
    AST *c=a->down;
    for (int i=0; c!=NULL && i<n; i++) c=c->right;
    return c;
} 

/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
    if (a==NULL) return;

    cout<<a->kind;
    if (a->text!="") cout<<"("<<a->text<<")";
    cout<<endl;

    AST *i = a->down;
    while (i!=NULL && i->right!=NULL) {
        cout<<s+"  \\__";
        ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
        i=i->right;
    }

    if (i!=NULL) {
        cout<<s+"  \\__";
        ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
        i=i->right;
    }
}

/// print AST 
void ASTPrint(AST *a)
{
    while (a!=NULL) {
        cout<<" ";
        ASTPrintIndent(a,"");
        a=a->right;
    }
}

int main() {
    AST *root = NULL;
    ANTLR(plumber(&root), stdin);
    ASTPrint(root);
}
>>

#lexclass START
#token SPACE "[\ \n]" << zzskip();>>
#token TUNE "TUBE"
#token CONNECTOR "CONNECTOR"
#token SPLIT "SPLIT"
#token MERGE "MERGE"
#token LENGHT "LENGTH"
#token DIAMETER "DIAMETER"
#token WHILE "WHILE"
#token ENDWHILE
#token NOT "NOT"
#token AND "AND"
#token OR "OR"
#token LPAREN "("
#token RPAREN ")"
#token COMMA ","
#token PUSH "PUSH"
#token POP "POP"
#token ARRAY "TUBEVECTOR OF"
#token FULL "FULL"
#token EMPTY "EMPTY"
#token ASSIG "="
#token GT ">"
#token LT "<"
#token EQ "=="
#token NUM "[0-9]+"
#token ID "[a-zA-Z]"

plumber: (op)* <<#0=createASTlist(_sibling);>>;

op : ID ASSIG^ funcTube
    | LPAREN ID COMMA ID RPAREN ASSIG^ SPLIT funcTube
    ;

funcConnector: CONNECTOR^ expr;

funcTube : ID
    | MERGE^ funcTube funcConnector funcTube
    | TUBE^ expr expr
    ;

expr : NUM;


/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   /opt/pccts/bin/antlr -gt plumber.g
 *
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string>
#include <iostream>
#include <vector>
using namespace std;

typedef unsigned int uint;

// struct to store information about tokens
struct Attrib {
  string kind;
  string text;
};

// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr, int ttype, char *textt);

class Prueba {
  public:
  ~Prueba() {
    cout << "Destruyendo prueba1" << endl;
  }
};

class Data {
  public:
  virtual void print() = 0;
  virtual ~Data() {
    
    }
};

class SimpleData : public Data {
  protected:
  uint diameter;
  public:
  SimpleData(uint diameter) : diameter(diameter) {
  }
  virtual ~SimpleData() {
    
    }
  
    uint getDiameter() {
    return diameter;
  }
};

class Tube : public SimpleData {
  uint length;
  public:
  Tube(uint length, uint diameter) : length(length), SimpleData(diameter) {
  }
  
    uint getLength() {
    return length;
  }
  
    void print() {
    cout << "Tube of length: " << length << " and diameter: " << diameter << endl;
  }
  
};

class Connector : public SimpleData {
  public:
  Connector(uint diameter) : SimpleData(diameter) {
  }
  
    void print() {
    cout << "Connector of diameter: " << diameter << endl;
  }
};

class Vector : public Data {
  vector<Tube> vec;
  Prueba p;
  
public:
  Vector(uint size) {
    vec.reserve(size);
    cout << size << " " << vec.capacity() << endl;
  }
  
    bool full() {
    return vec.size() == vec.capacity();
  }
  bool empty() {
    return vec.size() > 0;
  }
  
    uint length() {
    return vec.size();
  }
  void push(const Tube& tube) {
    if (vec.size() < vec.capacity()) {
      vec.push_back(tube);
    }
    else {
      cerr << "Trying to push a full tube vector" << endl;
      exit(-1);
    }
  }
  Tube pop() {
    Tube last =  vec[vec.size()-1];
    vec.pop_back();
    return last;
  }
  
    void print() {
    cout << "Vector of tubes: " << endl;
    for (Tube& tube : vec) {
      cout << "\t";
      tube.print();
    }
  }
};
#define GENAST

#include "ast.h"

#define zzSET_SIZE 4
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO

#include <cstdlib>
#include <cmath>
#include <map>
#include <typeinfo>

map<string, Data*> vars;
typedef map<string, Data*>::iterator Iterator;

// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  switch (type) {
    case ID:
    attr->text = text;
    attr->kind = "ID";
    break;
    case NUM:
    attr->text = text;
    attr->kind = "NUM";
    break;
    case ARRAY:
    attr->kind = "ARRAY";
    attr->text = "";
    break;
    default:
    attr->kind = text;
    attr->text = "";
    break;
  }
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
  AST* as=new AST;
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
void ASTPrintIndent(AST *a,string s) {
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
void ASTPrint(AST *a) {
  while (a!=NULL) {
    cout << " ";
    ASTPrintIndent(a,"");
    a=a->right;
  }
}

uint getLength(AST* child) {
  Iterator it = vars.find(child->text);
  if (it != vars.end()) {
    Tube* tube = dynamic_cast<Tube*>(it->second);
    if (tube) return tube->getLength();
    else {
      cout << "Wrong type, only tubes have length but " << child->text << " is a " << typeid(*it->second).name() << " instance" << endl;
      exit(-1);
    }
  }
  else {
    cout << "Var " << child->text << " does not exist" << endl;
    exit(-1);
  }
}

uint getDiameter(AST* child) {
  Iterator it = vars.find(child->text);
  if (it != vars.end()) {
    SimpleData* data = dynamic_cast<SimpleData*>(it->second);
    if (data) return data->getDiameter();
    
        cout << "Wrong type, only tubes and connectors have diameter but " << child->text << " is a " << typeid(*it->second).name() << " instance" << endl;
    exit(-1);
  }
  cout << "Var " << child->text << " does not exist" << endl;
  exit(-1);
}

int evaluateExpresion(AST* root) {
  if (root->kind == "NUM") {
    return stoi(root->text);
  }
  else if (root->kind == "+") {
    return evaluateExpresion(root->down)+evaluateExpresion(root->down->right);
  }
  else if (root->kind == "-") {
    return evaluateExpresion(root->down)-evaluateExpresion(root->down->right);
  }
  else if (root->kind == "*") {
    return evaluateExpresion(root->down)*evaluateExpresion(root->down->right);
  }
  else if (root->kind == "LENGTH") {
    return getLength(root->down);
  }
  else if (root->kind == "DIAMETER") {
    return getDiameter(root->down);
  }
}

void execute(AST* root) {
  if (root->kind == "=") {
    AST* childLeft = root->down;
    AST* childRight = childLeft->right;
    string& varName = childLeft->text;
    string& assignType = childRight->kind;
    if (assignType == "TUBE") {
      AST* lengthAST = childRight->down;
      AST* diameterAST = lengthAST->right;
      Iterator it = vars.lower_bound(varName);
      if (it != vars.end() and varName == it->first) {
        delete it->second;
        it->second = new Tube(evaluateExpresion(lengthAST), evaluateExpresion(diameterAST));
      }
      else vars.insert(it, make_pair(varName, new Tube(evaluateExpresion(lengthAST), evaluateExpresion(diameterAST))));
    }
    else if (assignType == "CONNECTOR") {
      Iterator it = vars.lower_bound(varName);
      if (it != vars.end() and varName == it->first) {
        delete it->second;
        it->second = new Connector(evaluateExpresion(childRight->down));
      }
      else vars.insert(it, make_pair(varName, new Connector(evaluateExpresion(childRight->down))));
    }
    else if (assignType == "ARRAY") {
      Iterator it = vars.lower_bound(varName);
      if (it != vars.end() and varName == it->first) {
        delete it->second;
        it->second = new Vector(evaluateExpresion(childRight->down));
      }
      else vars.insert(it, make_pair(varName, new Vector(evaluateExpresion(childRight->down))));
    }
  }
}

void executeList(AST* root) {
  AST* child = root->down;
  while (child != NULL) {
    execute(child);
    child = child->right;
  }
}

int main() {
  AST *root = NULL;
  ANTLR(plumber(&root), stdin);
  //ASTPrint(root);
  executeList(root);
  
    for (pair<const string, Data*>& var : vars) {
    cout << var.first << " is a ";
    var.second->print();
  }
}

void
#ifdef __USE_PROTOS
plumber(AST**_root)
#else
plumber(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  listOp(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(1);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x1);
  }
}

void
#ifdef __USE_PROTOS
listOp(AST**_root)
#else
listOp(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x2) ) {
      op(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x4);
  }
}

void
#ifdef __USE_PROTOS
op(AST**_root)
#else
op(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==ID) ) {
    zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    zzmatch(ASSIG); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      if ( (setwd1[LA(1)]&0x8) ) {
        funcTube(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {
        if ( (LA(1)==CONNECTOR) ) {
          funcConnector(zzSTR); zzlink(_root, &_sibling, &_tail);
        }
        else {
          if ( (LA(1)==ARRAY) ) {
            funcArray(zzSTR); zzlink(_root, &_sibling, &_tail);
          }
          else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==LPAREN) ) {
      zzmatch(LPAREN);  zzCONSUME;
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(COMMA);  zzCONSUME;
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(RPAREN);  zzCONSUME;
      zzmatch(ASSIG); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      funcSplit(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==WHILE) ) {
        zzmatch(WHILE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(LPAREN);  zzCONSUME;
        boolExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzmatch(RPAREN);  zzCONSUME;
        listOp(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzmatch(ENDWHILE);  zzCONSUME;
      }
      else {
        if ( (LA(1)==DIAMETER) ) {
          zzmatch(DIAMETER); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          zzmatch(LPAREN);  zzCONSUME;
          zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
          zzmatch(RPAREN);  zzCONSUME;
        }
        else {
          if ( (LA(1)==LENGTH) ) {
            zzmatch(LENGTH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
            zzmatch(LPAREN);  zzCONSUME;
            zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
            zzmatch(RPAREN);  zzCONSUME;
          }
          else {
            if ( (LA(1)==PUSH) ) {
              zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
              zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
              funcTube(zzSTR); zzlink(_root, &_sibling, &_tail);
            }
            else {
              if ( (LA(1)==POP) ) {
                zzmatch(POP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
                zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
                zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
              }
              else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
            }
          }
        }
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x10);
  }
}

void
#ifdef __USE_PROTOS
funcSplit(AST**_root)
#else
funcSplit(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(SPLIT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  funcTube(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x20);
  }
}

void
#ifdef __USE_PROTOS
funcArray(AST**_root)
#else
funcArray(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ARRAY); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  aritmeticExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x40);
  }
}

void
#ifdef __USE_PROTOS
funcConnector(AST**_root)
#else
funcConnector(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(CONNECTOR); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  aritmeticExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x80);
  }
}

void
#ifdef __USE_PROTOS
funcTube(AST**_root)
#else
funcTube(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==ID) ) {
    zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==MERGE) ) {
      zzmatch(MERGE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      funcTube(zzSTR); zzlink(_root, &_sibling, &_tail);
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        if ( (LA(1)==CONNECTOR) ) {
          funcConnector(zzSTR); zzlink(_root, &_sibling, &_tail);
        }
        else {
          if ( (LA(1)==ID) ) {
            zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp2);
        }
      }
      funcTube(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==TUBE) ) {
        zzmatch(TUBE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        aritmeticExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
        aritmeticExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x1);
  }
}

void
#ifdef __USE_PROTOS
boolExpr(AST**_root)
#else
boolExpr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  andExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==OR) ) {
      zzmatch(OR); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      andExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x2);
  }
}

void
#ifdef __USE_PROTOS
andExpr(AST**_root)
#else
andExpr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  notFunc(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==AND) ) {
      zzmatch(AND); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      notFunc(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x4);
  }
}

void
#ifdef __USE_PROTOS
notFunc(AST**_root)
#else
notFunc(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==NOT) ) {
      zzmatch(NOT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (setwd2[LA(1)]&0x8) ) {
      }
      else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  boolFunc(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x10);
  }
}

void
#ifdef __USE_PROTOS
boolFunc(AST**_root)
#else
boolFunc(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd2[LA(1)]&0x20) ) {
    aritmeticExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      while ( (setwd2[LA(1)]&0x40) ) {
        {
          zzBLOCK(zztasp3);
          zzMake0;
          {
          if ( (LA(1)==GT) ) {
            zzmatch(GT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {
            if ( (LA(1)==LT) ) {
              zzmatch(LT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
            }
            else {
              if ( (LA(1)==EQ) ) {
                zzmatch(EQ); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
              }
              else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
            }
          }
          zzEXIT(zztasp3);
          }
        }
        aritmeticExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzLOOP(zztasp2);
      }
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==FULL) ) {
      zzmatch(FULL); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(LPAREN);  zzCONSUME;
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(RPAREN);  zzCONSUME;
    }
    else {
      if ( (LA(1)==EMPTY) ) {
        zzmatch(EMPTY); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(LPAREN);  zzCONSUME;
        zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(RPAREN);  zzCONSUME;
      }
      else {
        if ( (LA(1)==LPAREN) ) {
          zzmatch(LPAREN);  zzCONSUME;
          boolExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
          zzmatch(RPAREN);  zzCONSUME;
        }
        else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x80);
  }
}

void
#ifdef __USE_PROTOS
aritmeticExpr(AST**_root)
#else
aritmeticExpr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  mulExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd3[LA(1)]&0x1) ) {
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        if ( (LA(1)==MINUS) ) {
          zzmatch(MINUS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==ADD) ) {
            zzmatch(ADD); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp3);
        }
      }
      mulExpr(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x2);
  }
}

void
#ifdef __USE_PROTOS
mulExpr(AST**_root)
#else
mulExpr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  numFunc(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==MUL) ) {
      zzmatch(MUL); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      numFunc(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x4);
  }
}

void
#ifdef __USE_PROTOS
numFunc(AST**_root)
#else
numFunc(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==NUM) ) {
    zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==LENGTH) ) {
      zzmatch(LENGTH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(LPAREN);  zzCONSUME;
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(RPAREN);  zzCONSUME;
    }
    else {
      if ( (LA(1)==DIAMETER) ) {
        zzmatch(DIAMETER); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(LPAREN);  zzCONSUME;
        zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(RPAREN);  zzCONSUME;
      }
      else {zzFAIL(1,zzerr9,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x8);
  }
}

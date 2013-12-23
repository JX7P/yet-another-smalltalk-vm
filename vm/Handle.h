#ifndef HANDLE_H
#define HANDLE_H

#include "Object.h"
#include "Dictionary.h"
#include <string.h>

#define REMEMBER_SCOPE_POSITION 0

typedef struct Handle {
	void *object;
	struct Handle *prev;
	struct Handle *next;
} Handle;

typedef struct HandleScope {
	struct HandleScope *parent;
	Object handles[1024];
	size_t size;
#if REMEMBER_SCOPE_POSITION
	char *file;
	size_t line;
#endif
} HandleScope;

typedef struct {
	Handle *current;
} HandlesIterator;

typedef struct {
	HandleScope *current;
} HandleScopeIterator;

typedef struct {
	Object *nil;
	Object *false;
	Object *true;
	Class *MetaClass;
	Class *UndefinedObject;
	Class *True;
	Class *False;
	Class *SmallInteger;
	Class *Symbol;
	Class *Character;
	//Class *Float;
	Class *String;
	Class *Array;
	Class *ByteArray;
	Class *Association;
	Class *Dictionary;
	Class *OrderedCollection;
	Class *Class;
	Class *TypeFeedback;
	Class *CompiledMethod;
	Class *CompiledBlock;
	Class *SourceCode;
	Class *FileSourceCode;
	Class *Block;
	Class *Message;
	Class *MethodContext;
	Class *BlockContext;
	Class *ExceptionHandler;
	Class *ClassNode;
	Class *MethodNode;
	Class *BlockNode;
	Class *BlockScope;
	Class *ExpressionNode;
	Class *MessageExpressionNode;
	Class *NilNode;
	Class *TrueNode;
	Class *FalseNode;
	Class *VariableNode;
	Class *IntegerNode;
	Class *CharacterNode;
	Class *SymbolNode;
	Class *StringNode;
	Class *ArrayNode;
	Class *ParseError;
	Class *UndefinedVariableError;
	Class *RedefinitionError;
	Class *ReadonlyVariableError;
	Class *InvalidPragmaError;
	Class *IoError;
	Dictionary *Smalltalk;
	Array *SymbolTable;
	String *initializeSymbol;
	String *finalizeSymbol;
	String *valueSymbol;
	String *value_Symbol;
	String *valueValueSymbol;
	String *doesNotUnderstandSymbol;
	String *cannotReturnSymbol;
	String *handlesSymbol;
	String *generateBacktraceSymbol;
} SmalltalkHandles;

extern Handle *HeapHandles;
extern SmalltalkHandles Handles;
extern HandleScope *ScopeTail;

static void *closeHandleScope(HandleScope *scope, void *handle);
static void *scopeHandle(void *object);
static void *persistHandle(void *handle);
static void *handle(void *object);
void freeHandle(void *handle);
void freeHandles(void);
Object *newObject(Class *class, size_t size);
static Value getTaggedPtr(void *handle);
Object *copyResizedObject(Object *object, size_t newSize);

void initHandlesIterator(HandlesIterator *iterator);
_Bool handlesIteratorHasNext(HandlesIterator *iterator);
Object *handlesIteratorNext(HandlesIterator *iterator);

void initHandleScopeIterator(HandleScopeIterator *iterator);
_Bool handleScopeIteratorHasNext(HandleScopeIterator *iterator);
HandleScope *handleScopeIteratorNext(HandleScopeIterator *iterator);


#if REMEMBER_SCOPE_POSITION
	#define openHandleScope(scope) _openHandleScope(scope, __FILE__, __LINE__)
	static void _openHandleScope(HandleScope *scope, char *file, size_t line)
	{
		memset(scope, 0, sizeof(*scope));
		scope->parent = ScopeTail;
		scope->file = file;
		scope->line = line;
		ScopeTail = scope;
	}
#else
	static void openHandleScope(HandleScope *scope)
	{
		memset(scope, 0, sizeof(*scope));
		scope->parent = ScopeTail;
		ScopeTail = scope;
	}
#endif


static void *closeHandleScope(HandleScope *scope, void *handle)
{
	ASSERT(ScopeTail == scope);
	ScopeTail = ScopeTail->parent;
	if (handle != NULL) {
		ASSERT(ScopeTail != NULL);
		return scopeHandle(((Object *) handle)->raw);
	}
	return NULL;
}


static void *scopeHandle(void *object)
{
	ASSERT(ScopeTail != NULL);
	ASSERT(ScopeTail->size < 256);
	Object *handle = &ScopeTail->handles[ScopeTail->size++];
	handle->raw = object;
	return handle;
}


static void *persistHandle(void *object)
{
	return handle(((Object *) object)->raw);
}


static void *handle(void *object)
{
	Handle *handle = malloc(sizeof(Handle));
	ASSERT(handle != NULL);
	handle->object = object;
	handle->prev = NULL;
	handle->next = HeapHandles;
	HeapHandles = handle;
	return (void *) handle;
}


static Value getTaggedPtr(void *handle)
{
	return tagPtr(((Object *) handle)->raw);
}

#endif

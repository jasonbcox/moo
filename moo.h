
#ifndef MOO_C_H
#define MOO_C_H

// ------------------------------------------------------------
// ---- moo.h - v1.1.1 ----------------------------------------
// ------------------------------------------------------------
// ---- MACROS for OBJECT ORIENTED-ness ( in C ) --------------
// ------------------------------------------------------------

// ------------------------------------------------------------
// Author: Jason Cox
// Email: contact@jasonbcox.com
// Date of Last Change: 2012-10-23
// ------------------------------------------------------------

// ------------------------------------------------------------
// ---- LICENSE INFORMATION -----------------------------------
// ------------------------------------------------------------
// You may use moo.h as long as you appropriately and visibly
// credit the authors listed above.  moo.h may be used for any
// purpose, commercial or otherwise.  THIS CODE IS PROVIDED
// "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
// ------------------------------------------------------------

// ------------------------------------------------------------
// The goal of this library is to enable faster code
// development in C by providing the following:
//		* Function members for structs
//		* 'this' variable for function members
//		* A library-independent doubly-linked list structure
//			with basic functionality
// ------------------------------------------------------------

// ------------------------------------------------------------
// ---- Version History ---------------------------------------
// ------------------------------------------------------------
// Versioning Scheme: vM.m.d
//		M - Major Version Number
//			(Important Additions and/or Necessary Bug Fixes)
//		m - Minor Version Number
//			(Minor Additions and/or Insignificant Bug Fixes)
//		d - Documentation Addition Only
// ------------------------------------------------------------
// 2012-2-21: v1.0.0
//		* Finished base features: MOO_ERROR, MOO_MALLOC, C,
//				NEW, STRUCT_DEF, FUNC_THIS
//		* Finished built-in type: MOO_LIST_DEF
// 2012-2-22 v1.0.1
//		* Added documentation
// 2012-10-23 v1.1.1
//		* Added STRUCT_END per the suggestion of Kenneth Perry,
//			as the non-matching brackets messed with some IDEs.
// ------------------------------------------------------------


// ------------------------------------------------------------
// ---- MOO Base Functionality --------------------------------
// ------------------------------------------------------------
// ---- "There is No Cow Level" -------------------------------
// ------------------------------------------------------------

// Default MOO_ERROR
#ifndef MOO_ERROR
#include <error.h>
#define MOO_ERROR( error_string )		\
do {									\
	error( 1, 0, error_string );		\
} while(0)

#endif

// Default MOO_MALLOC
#ifndef MOO_MALLOC
#include <stdlib.h>
#define MOO_MALLOC( T, N )									\
do {														\
	N = (T*)malloc( sizeof( T ) );							\
	if ( N == NULL ) MOO_ERROR( "MOO_MALLOC failed!" );		\
} while(0)

#endif

// Always use C() to make function calls for moo-based structs
// How to use C() and NEW():
/*	// Assuming you defined a moo-based struct previously
	my_struct * mine;
	NEW( my_struct, mine );
	C(mine)->func(args);
*/
#define C( O ) ( (O)->_call( (O) ) )

// Initializes a new moo-based object (very similar to C++'s 'new')
// T = definition type (the moo-based struct)
// N = variable name
// IMPORTANT: You must initialize T * N; at the top of the function where this is used
#define NEW( T, N )					\
do {								\
	MOO_MALLOC( T, N );				\
	N->_call = &( T##_call );		\
	T##_init( N );					\
	T##_C_safety = 0;				\
} while(0)

// Define a new moo-based struct
// How to use STRUCT_DEF:
/*
	STRUCT_DEF( my_struct )
		int value;					<- Any member variables you want
		void (*func)(void);			<- Any member functions you want
	STRUCT_END
	
	void my_struct_func() {
		my_struct * this;			<- Make sure to define 'this' before using it
		FUNC_THIS( my_struct );		<- Make sure to call FUNC_THIS whether you use 'this' or not
		this->value++;				<- The rest of the function code
	}
	
	void my_struct_init( my_struct * object ) {
		object->value = 0;						<- Any default member values you want
		object->func = &( my_struct_func );		<- Make sure to initialize all member functions
	}
*/
#define STRUCT_DEF( S )																\
	struct _##S;																	\
	typedef struct _##S S;															\
																					\
	S * S##_this;																	\
	int S##_C_safety;																\
	S * S##_call( S * this ) { S##_this = this; S##_C_safety = 1; return this; }	\
																					\
	struct _##S {																	\
		S * (*_call)( S * );
// end of STRUCT_DEF

#define STRUCT_END };

// Defines the variable 'this' in a moo-based struct's member function code
// It is smart to put this at the top of all member functions because it also checks to make sure
// the function was called with C().  Not calling the function via C() can result in undefined behavior.
// IMPORTANT: You must define S * this; at the top of the function where this is used!
#define FUNC_THIS( S )																									\
do {																													\
	if ( (!( S##_C_safety )) || ( S##_this == NULL ) ) { MOO_ERROR("MOO struct member function called without C()"); }	\
	this = S##_this;																									\
	S##_C_safety = 0;																									\
} while(0)

// ------------------------------------------------------------
// ---- MOO Built-in Types ------------------------------------
// ------------------------------------------------------------
// "If you only knew the power of the Dark Side." -- Darth Vader
// ------------------------------------------------------------

// Generates a 2 structs to make a doubley-linked list of type T, with a typedef called N
// To use the typedef N, N##_init( N * object ) must be called to initialize the entire structure
#define MOO_LIST_DEF( T, N )			\
	STRUCT_DEF( N##_node )				\
		T * item;						\
		N##_node * previous;			\
		N##_node * next;				\
	};									\
											\
	void N##_node_init( N##_node * O ) {	\
		O->item = NULL;						\
		O->previous = NULL;					\
		O->next = NULL;						\
	}										\
											\
	STRUCT_DEF( N )								\
		N##_node * front;						\
		N##_node * back;						\
		unsigned int size;						\
												\
		N##_node * (*get_node)( unsigned int );	\
		T * (*get)( unsigned int );				\
												\
		void (*insert)( N##_node *, T * );		\
		T * (*delete_node)( N##_node * );		\
		T * (*remove)( unsigned int );			\
												\
		void (*push_front)( T * );				\
		T * (*pop_front)();						\
												\
		void (*push_back)( T * );				\
		T * (*pop_back)();						\
	};											\
													\
	N##_node * N##_get_node( unsigned int index ) {	\
		unsigned int i = 0;							\
		N##_node * node;							\
		N * this;									\
		FUNC_THIS( N );								\
		if ( index >= this->size ) return NULL;		\
		node = this->front;							\
		while ( i < index ) {						\
			node = node->next;						\
			i++;									\
		}											\
		return node;								\
	}												\
												\
	T * N##_get( unsigned int index ) {			\
		N##_node * node;						\
		N * this;								\
		FUNC_THIS( N );							\
		node = C(this)->get_node( index );		\
		if ( node != NULL ) {					\
			return node->item;					\
		} else {								\
			return NULL;						\
		}											\
	}													\
															\
	void N##_insert( N##_node * insert_after, T * item ) {	\
		N##_node * newNode;									\
		N * this;											\
		FUNC_THIS( N );										\
		NEW( N##_node, newNode );							\
		newNode->item = item;								\
		if ( insert_after == NULL ) {						\
			newNode->previous = NULL;						\
			newNode->next = this->front;					\
			if ( this->front != NULL ) {					\
				this->front->previous = newNode;			\
			} else {										\
				this->back = newNode;						\
			}												\
			this->front = newNode;							\
		} else {											\
			newNode->previous = insert_after;				\
			newNode->next = insert_after->next;				\
			if ( insert_after->next != NULL ) {				\
				insert_after->next->previous = newNode;		\
			} else {										\
				this->back = newNode;						\
			}												\
			insert_after->next = newNode;					\
		}													\
		this->size++;										\
	}														\
														\
	T * N##_delete_node( N##_node * node ) {			\
		T * r;											\
		N * this;										\
		FUNC_THIS( N );									\
		if ( node != NULL ) {							\
			r = node->item;								\
			if ( node->previous == NULL ) {				\
				this->front = node->next;				\
			} else {									\
				node->previous->next = node->next;		\
			}											\
			if ( node->next == NULL ) {					\
				this->back = node->previous;			\
			} else {									\
				node->next->previous = node->previous;	\
			}											\
			this->size--;								\
			free( node );								\
			return r;									\
		} else {										\
			return NULL;								\
		}												\
	}												\
												\
	T * N##_remove( unsigned int index ) {		\
		N##_node * rNode;						\
		N * this;								\
		FUNC_THIS( N );							\
		rNode = C(this)->get_node( index );		\
		return C(this)->delete_node( rNode );	\
	}											\
												\
	void N##_push_front( T * item ) {			\
		N * this;								\
		FUNC_THIS( N );							\
		C(this)->insert( NULL, item );			\
	}											\
													\
	T * N##_pop_front() {							\
		N * this;									\
		FUNC_THIS( N );								\
		return C(this)->delete_node( this->front );	\
	}												\
													\
	void N##_push_back( T * item ) {				\
		N * this;									\
		FUNC_THIS( N );								\
		C(this)->insert( this->back, item );		\
	}												\
													\
	T * N##_pop_back() {							\
		N * this;									\
		FUNC_THIS( N );								\
		return C(this)->delete_node( this->back );	\
	}												\
													\
													\
	void N##_init( N * O ) {						\
		O->front = NULL;							\
		O->back = NULL;								\
		O->size = 0;								\
													\
		O->get_node = &( N##_get_node );			\
		O->get = &( N##_get );						\
													\
		O->insert = &( N##_insert );				\
		O->delete_node = &( N##_delete_node );		\
		O->remove = &( N##_remove );				\
													\
		O->push_front = &( N##_push_front );		\
		O->pop_front = &( N##_pop_front );			\
													\
		O->push_back = &( N##_push_back );			\
		O->pop_back = &( N##_pop_back );			\
	}

// ------------------------------------------------------------
// "I suppose you think that was terribly clever." -- Gandalf
// ------------------------------------------------------------

#endif

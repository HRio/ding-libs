/*
    COLLECTION LIBRARY

    Header file for collection interface.

    Copyright (C) Dmitri Pal <dpal@redhat.com> 2009

    Collection Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Collection Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Collection Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COLLECTION_H
#define COLLECTION_H

#ifndef EOK
#define EOK 0
#endif

#define COL_TYPE_STRING          0x00000001 /* For elements of type string the
                                               lenght includes the trailing 0 */
#define COL_TYPE_BINARY          0x00000002
#define COL_TYPE_INTEGER         0x00000004
#define COL_TYPE_UNSIGNED        0x00000008
#define COL_TYPE_LONG            0x00000010
#define COL_TYPE_ULONG           0x00000020
#define COL_TYPE_DOUBLE          0x00000040
#define COL_TYPE_BOOL            0x00000080
#define COL_TYPE_COLLECTION      0x00000100 /* The item of this type denotes
                                               that starting element of a
                                               collection */
#define COL_TYPE_COLLECTIONREF   0x00000200 /* An item of this type is a pointer
                                               to an existing external
                                               collection */
#define COL_TYPE_END             0x10000000 /* Special type that denotes the end
                                               of the collection. Useful when
                                               traversing collections */
#define COL_TYPE_ANY             0x0FFFFFFF /* Special type that denotes any.
                                               Useful when traversing
                                               collections */


/* Any data we deal with can't be longer than this */
/* FIXME - make it compile time option */
#define COL_MAX_DATA    65535

/* Default class for a free form collection */
#define COL_CLASS_DEFAULT      0

/* The modes that define how one collection can be added to another */

#define COL_ADD_MODE_REFERENCE 0    /* The collection will contain a pointer
                                       to another */
#define COL_ADD_MODE_EMBED     1    /* The collection will become part of
                                       another collection.
                                       After this operation the handle should
                                       not be used or freed.
                                       Can't be done more than once.
                                       If the collection is referenced by
                                       another collection, the operation will
                                       fail. */
#define COL_ADD_MODE_CLONE     2    /* Creates a deep copy of a collection with
                                       its sub collections */


/* Modes how the collection is traversed */
#define COL_TRAVERSE_DEFAULT  0x00000000  /* Traverse all items */
#define COL_TRAVERSE_ONELEVEL 0x00000001  /* Flag to traverse only top level
                                             ignored if the IGNORE flag is
                                             specified */
#define COL_TRAVERSE_END      0x00000002  /* Call the handler once more when the
                                             end of the collection is reached.
                                             Good for processing nested
                                             collections.
                                             Implicit for iterators unless
                                             the FLAT flag is specified */
#define COL_TRAVERSE_IGNORE   0x00000004  /* Ignore sub collections as if none
                                             is present */
#define COL_TRAVERSE_FLAT     0x00000008  /* Flatten the collection.
                                             FIXME: not implemented yet */

/* Additional iterator flags
 * NOTE: ignored by traverse functions */
#define COL_TRAVERSE_SHOWSUB  0x00010000  /* Include headers of sub collections
                                             By default iterators return just
                                             references and skips headers.
                                             Ignored if the ONELEVEL flag is
                                             specified and not ignored.
                                             Ignored if the FLAT flag is
                                             specified. */
#define COL_TRAVERSE_ONLYSUB  0x00020000  /* Show the header of the sub
                                             collection instead of reference.
                                             Ignored if the ONELEVEL flag is
                                             specified and not ignored.
                                             Ignored if the FLAT flag is
                                             specified. */

/* FIXME: move to event level - this does not belong to collection */
/* Time stamp property name */
#define TS_NAME         "stamp"
/* Time property name */
#define T_NAME          "time"

/* Match values */
#define COL_NOMATCH 0
#define COL_MATCH   1

/* Deapth for iteraor depth allocation block */
#define STACK_DEPTH_BLOCK   15

/* Public declaration of the private data */
#ifndef COLLECTION_PRIV_H
/* Structure that holds one property. */
struct collection_item;

/* Your implementation can assume that following members
 * will always be members of the collection_item.
 * but you should use get_item_xxx functions to get them.
 *   char *property;
 *   int property_len;
 *   int type;
 *   int length;
 *   void *data;
 */


/* Internal iterator structure */
struct collection_iterator;
#endif /* COLLECTION_PRIV_H */

/* IMPORTANT - the collection is a set of items of different type.
 * There is always a header item in any collection that starts the collection.
 * Most of the functions in the interface (unless explicitly stated otherwise)
 * assume that the collection_item * argument points to the header element.
 * Passing in elements extracted from the middle of a collection to functions
 * that expect header elements is illegal. There might be not enough checking
 * at the moment but this will be enforced in future versions of the library.

 * IMPORTANT - To better understand how collections work imagine travel bags.
 * They usually come in different sizes and one can put a bag in a bag when
 * they put away to the shelf in a garage or closet. Collection is such bag
 * except that you can put other bags into each other even if they are not
 * empty.
 * When you put items into a bag you do not see the contents of the bag.
 * You just hold the bag. How many other bags inside this bag you do not know.
 * But you might know that you put a "wallet" somewhere there.
 * You ask the bag you hold: "find my wallet and give it to me".
 * get_item function will return you the item that is your "wallet".
 * You can then change something or just get information about the item you
 * retrieved. But in most cases you do not the wallet itself. You want to get
 * something from the vallet or put something into it. IMO money would be an
 * obvious choice. To do this you use update_xxx_property functions.
 * There might be a bag somewhere deep and you might want to add something to
 * it. add_xxx_property_xxx functions allow you to specify sub collection you
 * want the item to be added to. If this sub collection argument is NULL top
 * level collection is assumed.
 * The search in the collections users a dotted notation to refer to an item (or
 * property). You can search for "wallet" and it will find any first instance of
 * the "wallet" in your luggage. But you might have two wallets. One is yours and
 * another is your significant other's. So you might say find "my.wallet".
 * It will find wallet in your bad (collection) named "my". This collection can
 * be many levels deep inside other collections. You do not need to know the
 * full path to get to it. But if you have the full path you can use the fill
 * path like this "luggage.newbags.my.wallet".
 * It is useful to be able to put bags into bags as well as get them out of each
 * other. When the collection is created the header keeps a reference count on
 * how many copies of the collection are known to the world. So one can put a
 * collection into collection and give up its access to it (embed) of still hold
 * to the reference. By embedding the collection the caller effectively gives
 * up its responsibility to destroy the collection after it is used.
 * By extracting reference from an internal collection the caller gains access
 * to the collection directly and thus has responsibility to destroy it after
 * use.
 */

/* Function that creates a named collection */
int create_collection(struct collection_item **ci, const char *name,
                      unsigned cclass);

/* Function that creates a named collection using a memory descriptor */
/* FIXME - function is a placeholder. It is not implemented yet.
 * will be added in future together with the definition of the
 * descriptor structure.
 * The purpose is to control the internal implementation of the collection
 * a) Use hash table for faster searches if the collection is expected to be large.
 * b) Define memory functions to use.
 */
/*
int create_collection_ex(struct collection_item **ci, const char *name,
                         unsigned cclass, struct cdescriptor *descrptor);
*/

/* Function that destroys a collection */
void destroy_collection(struct collection_item *ci);

/* Family of functions that add properties to a collection */
/* See details about subcollection argument above. */
/* Family includes the following convinience functions: */
/* Add a string property to collection. The length should include the
 * terminating 0 */
int add_str_property(struct collection_item *ci, const char *subcollection,
                     const char *property, char *string, int length);
/* Add a binary property to collection.  */
int add_binary_property(struct collection_item *ci, const char *subcollection,
                        const char *property, void *binary_data, int length);
/* Add an int property to collection. */
int add_int_property(struct collection_item *ci, const char *subcollection,
                     const char *property, int number);
/* Add an unsigned int property. */
int add_unsigned_property(struct collection_item *ci, const char *subcollection,
                          const char *property,unsigned int number);
/* Add a long property. */
int add_long_property(struct collection_item *ci, const char *subcollection,
                      const char *property, long number);
/* Add an unsigned long property. */
int add_ulong_property(struct collection_item *ci, const char *subcollection,
                       const char *property, unsigned long number);
/* Add a double property. */
int add_double_property(struct collection_item *ci, const char *subcollection,
                        const char *property,double number);
/* Add a bool property. */
int add_bool_property(struct collection_item *ci, const char *subcollection,
                      const char *property, unsigned char logical);

/* Add any property */
int add_any_property(struct collection_item *ci,    /* A collection of items */
                     const char *subcollection,     /* Subcollection */
                     const char *property,          /* Name */
                     int type,                      /* Data type */
                     void *data,                    /* Pointer to the data */
                     int length);                   /* Length of the data. For
                                                       strings it includes the
                                                       trailing 0 */

/* The functions that add an item and immediately return you this item
 * in the ret_ref parameter */
int add_str_property_with_ref(struct collection_item *ci,
                              const char *subcollection,
                              const char *property,
                              char *string, int length,
                              struct collection_item **ret_ref);
int add_binary_property_with_ref(struct collection_item *ci,
                                 const char *subcollection,
                                 const char *property,
                                 void *binary_data, int length,
                                 struct collection_item **ret_ref);
int add_int_property_with_ref(struct collection_item *ci,
                              const char *subcollection,
                              const char *property, int number,
                              struct collection_item **ret_ref);
int add_unsigned_property_with_ref(struct collection_item *ci,
                                   const char *subcollection,
                                   const char *property, unsigned int number,
                                   struct collection_item **ret_ref);
int add_long_property_with_ref(struct collection_item *ci,
                               const char *subcollection,
                               const char *property, long number,
                               struct collection_item **ret_ref);
int add_ulong_property_with_ref(struct collection_item *ci,
                                const char *subcollection,
                                const char *property, unsigned long number,
                                struct collection_item **ret_ref);
int add_double_property_with_ref(struct collection_item *ci,
                                 const char *subcollection,
                                 const char *property, double number,
                                 struct collection_item **ret_ref);
int add_bool_property_with_ref(struct collection_item *ci,
                               const char *subcollection,
                               const char *property, unsigned char logical,
                               struct collection_item **ret_ref);
int add_any_property_with_ref(struct collection_item *ci,
                              const char *subcollection,
                              const char *property,
                              int type, void *data, int length,
                              struct collection_item **ret_ref);

/* FIXME - does not belong here - move to other place */
/* Function to create a timestamp */
/* Automatically adds/updates time and timestamp properties in the
 * collection returning references */
int set_timestamp(struct collection_item *ci,
                  struct collection_item **timestr_ref,
                  struct collection_item **timeint_ref);


/* Update functions */
/* All update functions search the property using the search algorithm
 * described at the top of the header file.
 * Use same dotted notation to specify a property.
 */
/* Update a string property in the collection.
 * Length should include the terminating 0  */
int update_str_property(struct collection_item *ci, const char *property,
                        int mode_flags, char *string,int length);
/* Update a binary property in the collection.  */
int update_binary_property(struct collection_item *ci, const char *property,
                           int mode_flags, void *binary_data,int length);
/* Update an int property in the collection. */
int update_int_property(struct collection_item *ci, const char *property,
                        int mode_flags, int number);
/* Update an unsigned int property. */
int update_unsigned_property(struct collection_item *ci, const char *property,
                             int mode_flags, unsigned int number);
/* Update a long property. */
int update_long_property(struct collection_item *ci, const char *property,
                         int mode_flags, long number);
/* Update an unsigned long property. */
int update_ulong_property(struct collection_item *ci, const char *property,
                          int mode_flags, unsigned long number);
/* Update a double property. */
int update_double_property(struct collection_item *ci, const char *property,
                           int mode_flags, double number);
/* Update a double property. */
int update_bool_property(struct collection_item *ci, const char *property,
                         int mode_flags, unsigned char logical);


/* Update property in the collection */
int update_property(struct collection_item *ci, /* A collection of items */
                    const char *property_to_find,/* Name to match */
                    int type,                   /* Data type */
                    void *new_data,             /* Pointer to the data */
                    int length,                 /* Length of the data. For
                                                   strings, it includes the
                                                   trailing 0 */
                    int mode_flags);            /* How to traverse the collection  */




/* Add collection to collection */
int add_collection_to_collection(struct collection_item *ci,                   /* Collection handle to with we add another collection */
                                 const char *sub_collection_name,              /* Name of the sub collection to which
                                                                                  collection needs to be added as a property.
                                                                                  If NULL high level collection is assumed. */
                                 const char *as_property,                      /* Name of the collection property.
                                                                                  If NULL, same property as the name of
                                                                                  the collection being added will be used. */
                                 struct collection_item *collection_to_add,    /* Collection to add */
                                 int mode);                                    /* How this collection needs to be added */

/* Create a deep copy of the current collection. */
/* Referenced collections of the donor are copied as sub collections. */
int copy_collection(struct collection_item **collection_copy,
                    struct collection_item *collection_to_copy,
                    const char *name_to_use);

/* Signature of the callback that needs to be used when
   traversing a collection or looking for a specific item */
typedef int (*item_fn)(const char *property,   /* The name of the property will be passed in this parameter. */
                       int property_len, /* Length of the property name will be passed in this parameter. */
                       int type,         /* Type of the data will be passed in this parameter */
                       void *data,       /* Pointer to the data will be passed in this parameter */
                       int length,       /* Length of data will be passed in this parameter */
                       void *custom_dat, /* Custom data will be passed in this parameter */
                       int *stop);       /* Pointer to variable where the handler can put non zero to stop processing */

/* Function to traverse the entire collection including optionally sub collections */
int traverse_collection(struct collection_item *ci,    /* Collection to traverse */
                        int mode_flags,                /* Flags defining how to traverse */
                        item_fn item_handler,          /* Handler called for each item */
                        void *custom_data);            /* Custom data passed around */

/* Search function. Looks up an item in the collection based on the property.
   Actually it is a traverse function with spacial traversing logic.
   It is the responsibility of the handler to set something in the custom data
   to indicate that the item was found.
   Function will not return error if the item is not found.
   It is the responsibility of the calling application to check
   the data passed in custom_data and see if the item was found and
   that the action was performed.
 */
int get_item_and_do(struct collection_item *ci,  /* A collection of items */
                    const char *property_to_find,/* Name to match */
                    int type,                    /* Type filter */
                    int mode_flags,              /* How to traverse the collection */
                    item_fn item_handler,        /* Function to call when the item is found */
                    void *custom_data);          /* Custom data passed around */

/* Convenience function to get individual item */
/* Caller should be aware that this is not a copy of the item
 * but the pointer to actual item stored in the collection.
 * The returned pointer should never be altered or freed by caller of the function.
 * The caller should be sure that the collection does not go out of scope
 * while the pointer to its data is in use.
 * Working with the internals of the collection item structure directly
 * may cause problems in future if the internal implementation changes.
 */
int get_item(struct collection_item *ci,       /* Collection to find things in */
             const char *property_to_find,     /* Name to match */
             int type,                         /* Type filter */
             int mode_flags,                   /* How to traverse the collection */
             struct collection_item **item);   /* Found item */

/* Group of functions that allows retrieving individual elements of the collection_item
 * hiding the internal implementation.
 */
const char *get_item_property(struct collection_item *ci,int *property_len);
int get_item_type(struct collection_item *ci);
int get_item_length(struct collection_item *ci);
const void *get_item_data(struct collection_item *ci);

/* If you want to modify the item that you got as a result of iterating through collection
 * or by calling get_item(). If you want to rename item provide a new name in the property
 * argument. If you want the data to remain unchanged use 0 as length parameter.
 * If item is a reference or collection the call will return error.
 * Previous type and data of the item is destroyed.
 */
int modify_item(struct collection_item *item,
                const char *property,
                int type,
                void *data,
                int length);

/* Rename the item */
int modify_item_property(struct collection_item *item,
                         const char *property);

/* Convenience functions that wrap modify_item().
 * They always assign new value.
 * To rename the property just use modify_item_property();
 */
int modify_str_item(struct collection_item *item,
                    const char *property,
                    char *string,
                    int length);
int modify_binary_item(struct collection_item *item,
                       const char *property,
                       void *binary_data,
                       int length);
int modify_bool_item(struct collection_item *item,
                     const char *property,
                     unsigned char logical);
int modify_int_item(struct collection_item *item,
                    const char *property,
                    int number);
int modify_long_item(struct collection_item *item,
                     const char *property,
                     long number);
int modify_ulong_item(struct collection_item *item,
                      const char *property,
                      unsigned long number);
int modify_unsigned_item(struct collection_item *item,
                         const char *property,
                         unsigned number);
int modify_double_item(struct collection_item *item,
                       const char *property,
                       double number);

/* Delete property from the collection */
int delete_property(struct collection_item *ci,    /* A collection of items */
                    const char *property_to_find,  /* Name to match */
                    int type,                      /* Type filter */
                    int mode_flags);               /* How to traverse the collection  */


/* Convenience function to check if the property is indeed in the collection */
int is_item_in_collection(struct collection_item *ci,  /* Collection to find things in */
                          const char *property_to_find,/* Name to match */
                          int type,                    /* Type filter */
                          int mode_flags,              /* How to traverse the collection */
                          int *found);                 /* Boolean that turns to nonzero if the match is found */


/* Get collection - get a pointer to a collection included into another collection */
/* Delete extracted collection after use to decrease reference count. */
int get_collection_reference(struct collection_item *ci,          /* High level collection */
                             struct collection_item **acceptor,   /* The pointer that will accept extracted handle */
                             const char *collection_to_find);     /* Name to of the collection */

/* Get collection - if current item is a reference get a real collection from it. */
/* Delete extracted collection after use to decrease reference count. */
int get_reference_from_item(struct collection_item *ci,          /* Reference element of the high level collection */
                            struct collection_item **acceptor);   /* The pointer that will accept extracted handle */


/* Bind iterator to a collection */
int bind_iterator(struct collection_iterator **iterator, /* The iterator to bind */
                  struct collection_item *ci,            /* Collection to bind iterator to */
                  int mode_flags);                       /* How the collection needs to be iterated */

/* Unbind the iterator from the collection */
void unbind_iterator(struct collection_iterator *iterator);

/* Get items from the collection one by one following the tree */
int iterate_collection(struct collection_iterator *iterator,
                       struct collection_item **item);

/* Stop processing this subcollection and move to the next item in the
 * collection 'level' levels up.
 * The 'Level' parameter indicates how many levels up you want to jump.
 * If 0 - call is a no op.
 * If the depth is less than requested level function will return error EINVAL.
 */
int iterate_up(struct collection_iterator *iterator, int level);

/* How deep are we relative to the top level.*/
int get_iterator_depth(struct collection_iterator *iterator, int *depth);

/* FIXME - Do we need to be able to rewind iterator? */

/* Set collection class */
int set_collection_class(struct collection_item *item, /* Collection */
                         unsigned cclass);             /* Collection class */

/* Get collection class */
int get_collection_class(struct collection_item *item, /* Collection */
                         unsigned *cclass);            /* Collection class */


/* Get collection count */
int get_collection_count(struct collection_item *item, /* Collection */
                         unsigned *count);             /* Number of elements in
                                                        * this collection.
                                                        * Each subcollection is
                                                        * counted as 1 element.
                                                        * Header is also counted.
                                                        */

/* Convenience function to check if the collection is of the specific class */
/* In case of internal error assumes that collection is not of the right class */
int is_of_class(struct collection_item *item,  /* Collection */
                unsigned cclass);              /* Class of the collection */


/*
 * Series of collection functions that allow using collection as a stack or a FIFO
 */


/* Extract the item from the collection */
/* The header will not be considered for extraction. */
int extract_item(struct collection_item *ci,        /* Top collection */
                 const char *subcollection,         /* Sub collection */
                 int disposition,                   /* Which to extract */
                 const char *refprop,               /* Property to relate to */
                 int index,                         /* Index of the property to extract. See notes. */
                 int type,                          /* Type filter */
                 struct collection_item **ret_ref); /* Returns the reference back */

/* Similar extraction function as above just considers only one level. */
int extract_item_from_current(struct collection_item *ci,        /* Top collection */
                              int disposition,                   /* Which to extract */
                              const char *refprop,               /* Property to relate to */
                              int index,                         /* Index of the property to extract. See notes. */
                              int type,                          /* Type filter */
                              struct collection_item **ret_ref); /* Returns the reference back */

/* Insert item to the collection */
/* WARNING: Only use this function to insert items
 * that were extracted using extract_item().
 * NEVER use it with items that were returned
 * by get_item() or add_xxx_property_with_ref() or
 * with insert_xxx_property_with_ref().
 * The fundamental difference is that when you extracted item
 * using extract_item() it stops to be managed by a collection.
 * With such item you can:
 * a) Insert this item into another (or same) collection
 * b) Get item information using get_item_xxx() functions.
 * c) Destroy item using delete_item().
 * You are required to do either a) or c) with such item.
 */
int insert_item(struct collection_item *collection, /* Top collection */
                const char *subcollection,          /* Sub collection */
                struct collection_item *item,       /* Item to insert */
                int disposition,                    /* What should be the position of the item */
                const char *refprop,                /* Property to relate to */
                int index,                          /* Index of the property to extract. See notes. */
                unsigned flags);                    /* Flags that control naming issues */

/* Insert the item into the top level collection (similar to the function above)
 * but does not provide access to the sub collection.
 */
int insert_item_into_current(struct collection_item *collection,
                             struct collection_item *item,
                             int disposition,
                             const char *refprop,
                             int index,
                             unsigned flags);



/* Function to delete the item */
void delete_item(struct collection_item *item);

/* Possible dispositions for insert, extract and delete function(s).
 * Not all of these dispositions are implemented day one.
 * If disposition is not implemented the function
 * will return error ENOSYS.
 */
#define COL_DSP_END             0 /* Add property to the end of the collection */
                                  /* Extract or delete last property in collection */
#define COL_DSP_FRONT           1 /* Add property to the top of the collection */
                                  /* Extract or delete firat property in collection */
#define COL_DSP_BEFORE          2 /* Add property before other named property */
                                  /* Extract or delete property that is before given
                                   * property. If the given property is first
                                   * in the list ENOENT is returned.
                                   */
#define COL_DSP_AFTER           3 /* Add property immediately after other named property */
                                  /* Delete or extract property immediately after given
                                   * property. If the given property is last in the list
                                   * ENOENT is returned.
                                   */
#define COL_DSP_INDEX           4 /* Add, extract or delete property using index.
                                   * See notes below.
                                   */
/* NOTE ABOUT USING: COL_DSP_INDEX. */
/* The COL_DSP_INDEX adds the item as N-th item after header in the list.
 * Index is zero based.
 * If there are less than N items in the list the item is added to the end.
 * The index value of 0 means that the item will be added immediately
 * after the header. Index of 1 will mean that it is added after first data item and so on.
 *
 * In case of extraction or deletion the N-th item of the collection
 * will be extracted or deleted.
 * Index is zero based.
 * If there are less than N+1 items in the list the function will return ENOENT.
 */

/* The following three dispositions operate only with list of duplicate
 * properties that are going one after another.
 * In case of addition the property name is taken from the item
 * and the value refprop is ignored.
 * In case of extraction or deletion the property name is taken
 * from the refprop.
 */
#define COL_DSP_FIRSTDUP        5 /* Add property as a first dup of the given property */
                                  /* In case of extraction or deletion extracts or deletes
                                   * given property.
                                   */
#define COL_DSP_LASTDUP         6 /* Add property as a last dup of the given property */
                                  /* In case of extraction or deletion extracts or deletes
                                   * last duplicate property in the uninterrupted sequence of
                                   * properties with the same name.
                                   */
#define COL_DSP_NDUP            7 /* Add property as a N-th dup (0- based) of the given property. */
                                  /* In case of extraction or deletion extracts or deletes
                                   * N-th (0-based) duplicate of the given property.
                                   * If index is greater than number of duplicate
                                   * properties in sequence ENOENT is returned.
                                   * See more details below.
                                   */

/* Other dispositions might be possible in future. */

/* The COL_DSP_NDUP is used in case of the multi value property
 * to add a new property with the same name into specific place
 * in the list of properties with the same name.
 * The index of 0 will mean to add the property before the first instance of the property with the same name.
 * If the property does not exist ENOENT will be returned.
 * If the index is greater than the last property with the same name the item will be added
 * immediately after last property with the same name.
 */

/* Flags that can be used with insert functions */
#define COL_INSERT_NOCHECK      0   /* This is the default mode - no dup checks on insert */
#define COL_INSERT_DUPOVER      1   /* Check for dup name and overwrite - position ignored */
#define COL_INSERT_DUPOVERT     2   /* Check for dup name and type and overwrite - position ignored */
#define COL_INSERT_DUPERROR     3   /* Return error EEXIST if the entry with the same name exists */
#define COL_INSERT_DUPERRORT    4   /* Return error EEXIST if the entry with the same name and type exists */
#define COL_INSERT_DUPMOVE      5   /* Check for dups, overwrite, extracts and
                                     * then move to the position requested */
#define COL_INSERT_DUPMOVET     6   /* Check for dup name and type, overwrite, extracts
                                     * and then move to the position requested */

/* In future can be made more complex */

/* NOTE ABOUT FLAGS: Use of the DUP checking flags is costly since it requires a forward look up of the whole
 * collection before the item is inserted. Do not use it until it is absolutely necessary.
 */



/* The attributes in the collection are always added to the end.
 * The family of the insert_xxx(functions) provides
 * much more flexibility than add_xxx_property_xxx() functions.
 */

/* Insert property with reference */
int insert_property_with_ref(struct collection_item *ci,        /* A collection of items */
                             const char *subcollection,         /* Sub collection */
                             int disposition,                   /* Where to insert */
                             const char *refprop,               /* Property to relate to */
                             int index,                         /* Index of the property to add */
                             unsigned flags,                    /* Flags that control naming issues */
                             const char *property,              /* Name */
                             int type,                          /* Data type */
                             void *data,                        /* Pointer to the data */
                             int length,                        /* Length of the data. For
                                                                 * strings it includes the
                                                                 * trailing 0
                                                                 */
                             struct collection_item **ret_ref); /* Returns the reference back */


int insert_str_property(struct collection_item *ci,        /* A collection of items */
                        const char *subcollection,         /* Sub collection */
                        int disposition,                   /* Where to insert */
                        const char *refprop,               /* Property to relate to */
                        int index,                         /* Index of the property to add */
                        unsigned flags,                    /* Flags that control naming issues */
                        const char *property,              /* Name */
                        char *string,                      /* String */
                        int length);                       /* Length */

int insert_binary_property(struct collection_item *ci,        /* A collection of items */
                           const char *subcollection,         /* Sub collection */
                           int disposition,                   /* Where to insert */
                           const char *refprop,               /* Property to relate to */
                           int index,                         /* Index of the property to add */
                           unsigned flags,                    /* Flags that control naming issues */
                           const char *property,              /* Name */
                           void *binary_data,                 /* Binary data */
                           int length);                       /* Length */


int insert_int_property(struct collection_item *ci,        /* A collection of items */
                        const char *subcollection,         /* Sub collection */
                        int disposition,                   /* Where to insert */
                        const char *refprop,               /* Property to relate to */
                        int index,                         /* Index of the property to add */
                        unsigned flags,                    /* Flags that control naming issues */
                        const char *property,              /* Name */
                        int number);                       /* Integer */


int insert_unsinged_property(struct collection_item *ci,        /* A collection of items */
                             const char *subcollection,         /* Sub collection */
                             int disposition,                   /* Where to insert */
                             const char *refprop,               /* Property to relate to */
                             int index,                         /* Index of the property to add */
                             unsigned flags,                    /* Flags that control naming issues */
                             const char *property,              /* Name */
                             unsigned number);                  /* Unsigned */


int insert_long_property(struct collection_item *ci,        /* A collection of items */
                         const char *subcollection,         /* Sub collection */
                         int disposition,                   /* Where to insert */
                         const char *refprop,               /* Property to relate to */
                         int index,                         /* Index of the property to add */
                         unsigned flags,                    /* Flags that control naming issues */
                         const char *property,              /* Name */
                         long number);                      /* Long */

int insert_ulong_property(struct collection_item *ci,        /* A collection of items */
                          const char *subcollection,         /* Sub collection */
                          int disposition,                   /* Where to insert */
                          const char *refprop,               /* Property to relate to */
                          int index,                         /* Index of the property to add */
                          unsigned flags,                    /* Flags that control naming issues */
                          const char *property,              /* Name */
                          unsigned long number);             /* Unsigned long */

int insert_double_property(struct collection_item *ci,        /* A collection of items */
                           const char *subcollection,         /* Sub collection */
                           int disposition,                   /* Where to insert */
                           const char *refprop,               /* Property to relate to */
                           int index,                         /* Index of the property to add */
                           unsigned flags,                    /* Flags that control naming issues */
                           const char *property,              /* Name */
                           double number);                    /* Double */

int insert_bool_property(struct collection_item *ci,        /* A collection of items */
                         const char *subcollection,         /* Sub collection */
                         int disposition,                   /* Where to insert */
                         const char *refprop,               /* Property to relate to */
                         int index,                         /* Index of the property to add */
                         unsigned flags,                    /* Flags that control naming issues */
                         const char *property,              /* Name */
                         unsigned char logical);            /* Bool */



int insert_str_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                 const char *subcollection,         /* Sub collection */
                                 int disposition,                   /* Where to insert */
                                 const char *refprop,               /* Property to relate to */
                                 int index,                         /* Index of the property to add */
                                 unsigned flags,                    /* Flags that control naming issues */
                                 const char *property,              /* Name */
                                 char *string,                      /* String */
                                 int length,                        /* Length */
                                 struct collection_item **ret_ref); /* Returns the reference back */

int insert_binary_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                    const char *subcollection,         /* Sub collection */
                                    int disposition,                   /* Where to insert */
                                    const char *refprop,               /* Property to relate to */
                                    int index,                         /* Index of the property to add */
                                    unsigned flags,                    /* Flags that control naming issues */
                                    const char *property,              /* Name */
                                    void *binary_data,                 /* Binary data */
                                    int length,                        /* Length */
                                    struct collection_item **ret_ref); /* Returns the reference back */


int insert_int_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                 const char *subcollection,         /* Sub collection */
                                 int disposition,                   /* Where to insert */
                                 const char *refprop,               /* Property to relate to */
                                 int index,                         /* Index of the property to add */
                                 unsigned flags,                    /* Flags that control naming issues */
                                 const char *property,              /* Name */
                                 int number,                        /* Integer */
                                 struct collection_item **ret_ref); /* Returns the reference back */


int insert_unsinged_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                      const char *subcollection,         /* Sub collection */
                                      int disposition,                   /* Where to insert */
                                      const char *refprop,               /* Property to relate to */
                                      int index,                         /* Index of the property to add */
                                      unsigned flags,                    /* Flags that control naming issues */
                                      const char *property,              /* Name */
                                      unsigned number,                   /* Unsigned */
                                      struct collection_item **ret_ref); /* Returns the reference back */

int insert_long_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                  const char *subcollection,         /* Sub collection */
                                  int disposition,                   /* Where to insert */
                                  const char *refprop,               /* Property to relate to */
                                  int index,                         /* Index of the property to add */
                                  unsigned flags,                    /* Flags that control naming issues */
                                  const char *property,              /* Name */
                                  long number,                       /* Long */
                                  struct collection_item **ret_ref); /* Returns the reference back */

int insert_ulong_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                   const char *subcollection,         /* Sub collection */
                                   int disposition,                   /* Where to insert */
                                   const char *refprop,               /* Property to relate to */
                                   int index,                         /* Index of the property to add */
                                   unsigned flags,                    /* Flags that control naming issues */
                                   const char *property,              /* Name */
                                   unsigned long number,              /* Unsigned long */
                                   struct collection_item **ret_ref); /* Returns the reference back */

int insert_double_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                    const char *subcollection,         /* Sub collection */
                                    int disposition,                   /* Where to insert */
                                    const char *refprop,               /* Property to relate to */
                                    int index,                         /* Index of the property to add */
                                    unsigned flags,                    /* Flags that control naming issues */
                                    const char *property,              /* Name */
                                    double number,                     /* Double */
                                    struct collection_item **ret_ref); /* Returns the reference back */

int insert_bool_property_with_ref(struct collection_item *ci,        /* A collection of items */
                                  const char *subcollection,         /* Sub collection */
                                  int disposition,                   /* Where to insert */
                                  const char *refprop,               /* Property to relate to */
                                  int index,                         /* Index of the property to add */
                                  unsigned flags,                    /* Flags that control naming issues */
                                  const char *property,              /* Name */
                                  unsigned char logical,             /* Bool */
                                  struct collection_item **ret_ref); /* Returns the reference back */


#endif

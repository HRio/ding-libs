/*
    COLLECTION LIBRARY

    Collection unit test.

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


#include <stdio.h>
#include <string.h>
#include <errno.h>
#define TRACE_HOME
#include "trace.h"
#include "collection.h"
#include "collection_tools.h"


int ref_collection_test()
{
    struct collection_item *peer;
    struct collection_item *socket;
    char binary_dump[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    int found = 0;

    int error = EOK;

    TRACE_FLOW_STRING("ref_collection_test","Entry.");

    printf("\n\nREF TEST!!!.\n\n\n");
    printf("Creating PEER collection.\n");

    if((error=create_collection(&peer,"peer",0)) ||
       (error=add_str_property(peer,NULL,"hostname","peerhost.mytest.com",0)) ||
       (error=add_str_property(peer,NULL,"IPv4","10.10.10.10",12)) || /* Expect trailing zero to be truncated */
       (error=add_str_property(peer,NULL,"IPv6","bla:bla:bla:bla:bla:bla",0))) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(peer);
        return error;
    }

    printf("Creating SOCKET collection.\n");

    if((error=create_collection(&socket,"socket",0)) ||
       (error=add_int_property(socket,NULL,"id",1)) ||
       (error=add_long_property(socket,NULL,"packets",100000000L)) ||
       (error=add_binary_property(socket,NULL,"stack",binary_dump,sizeof(binary_dump)))) {
        destroy_collection(peer);
        destroy_collection(socket);
        printf("Failed to add property. Error %d\n",error);
        return error;
    }

    debug_collection(socket,COL_TRAVERSE_DEFAULT);
    debug_collection(peer,COL_TRAVERSE_DEFAULT);

    printf("Adding PEER collection to SOCKET collection as a reference named PEER\n");

    /* Embed peer host into the socket2 as reference */
    error = add_collection_to_collection(socket,NULL,"peer",peer,COL_ADD_MODE_REFERENCE);
    if(error) {
        destroy_collection(peer);
        destroy_collection(socket);
        printf("Failed to add collection to collection. Error %d\n",error);
        return error;
    }

    debug_collection(socket,COL_TRAVERSE_DEFAULT);
    debug_collection(peer,COL_TRAVERSE_DEFAULT);

    printf("About to destroy PEER\n");
    destroy_collection(peer);
    debug_collection(socket,COL_TRAVERSE_DEFAULT);

    printf("About to extract PEER\n");
    error = get_collection_reference(socket,&peer,"peer");
    if(error) {
        destroy_collection(socket);
        printf("Failed to extract collection. Error %d\n",error);
        return error;
    }

    debug_collection(socket,COL_TRAVERSE_DEFAULT);
    debug_collection(peer,COL_TRAVERSE_DEFAULT);
    destroy_collection(peer);

    debug_collection(socket,COL_TRAVERSE_DEFAULT);
    destroy_collection(socket);
    TRACE_FLOW_NUMBER("ref_collection_test. Returning",error);

    printf("\n\nEND OF REF TEST!!!.\n\n\n");

    return error;

}


int single_collection_test()
{
    struct collection_item *handle;
    int error = EOK;

    TRACE_FLOW_STRING("single_collection_test","Entry.");

    if((error=create_collection(&handle,"string_test",0)) ||
       (error=add_str_property(handle,NULL,"property_1","some data",0)) ||
       (error=add_str_property(handle,NULL,"property_2","some other data",2)) ||
       (error=add_str_property(handle,NULL,"property_3","more data",7))) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(handle);
        return error;
    }

    error = add_str_property(handle,NULL,"property 1","some data",0);
    if(error) printf("Expected error adding bad property to collection %d\n",error);
    else {
        printf("Expected error but got success\n");
        return -1;
    }

    error=add_double_property(handle,NULL,"double",0.253545);
    if(error) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(handle);
        return error;
    }

    error=update_double_property(handle,"double",COL_TRAVERSE_DEFAULT,1.999999);
    if(error) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(handle);
        return error;
    }
    printf("Created collection\n");

    /* Traverse collection */
    error = debug_collection(handle,COL_TRAVERSE_DEFAULT);
    if(error) {
        printf("Error debugging collection %d\n",error);
        return error;
    }
    error = print_collection(handle);
    if(error) {
        printf("Error printing collection %d\n",error);
        return error;
    }

    destroy_collection(handle);

    TRACE_FLOW_NUMBER("single_collection_test. Error: ",error);
    return error;
}

int add_collection_test()
{
    struct collection_item *peer;
    struct collection_item *socket;
    char binary_dump[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    int found = 0;

    int error = EOK;

    TRACE_FLOW_STRING("add_collection_test","Entry.");

    printf("\n\nADD TEST!!!.\n\n\n");
    printf("Creating PEER collection.\n");

    if((error=create_collection(&peer,"peer",0)) ||
       (error=add_str_property(peer,NULL,"hostname","peerhost.mytest.com",0)) ||
       (error=add_str_property(peer,NULL,"IPv4","10.10.10.10",12)) || /* Expect trailing zero to be truncated */
       (error=add_str_property(peer,NULL,"IPv6","bla:bla:bla:bla:bla:bla",0))) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(peer);
        return error;
    }

    printf("Creating SOCKET collection.\n");

    if((error=create_collection(&socket,"socket",0)) ||
       (error=add_int_property(socket,NULL,"id",1)) ||
       (error=add_long_property(socket,NULL,"packets",100000000L)) ||
       (error=add_binary_property(socket,NULL,"stack",binary_dump,sizeof(binary_dump)))) {
        destroy_collection(peer);
        destroy_collection(socket);
        printf("Failed to add property. Error %d\n",error);
        return error;
    }

    debug_collection(socket,COL_TRAVERSE_DEFAULT);
    debug_collection(peer,COL_TRAVERSE_DEFAULT);

    printf("Adding PEER collection to SOCKET collection as a reference named PEER\n");

    /* Embed peer host into the socket2 as reference */
    error = add_collection_to_collection(socket,NULL,"peer",peer,COL_ADD_MODE_REFERENCE);
    if(error) {
        destroy_collection(peer);
        destroy_collection(socket);
        printf("Failed to create collection. Error %d\n",error);
        return error;
    }

    debug_collection(socket,COL_TRAVERSE_DEFAULT);
    debug_collection(peer,COL_TRAVERSE_DEFAULT);
    destroy_collection(peer);
    debug_collection(socket,COL_TRAVERSE_DEFAULT);
    destroy_collection(socket);
    TRACE_FLOW_NUMBER("add_collection_test. Returning",error);
    return error;
}

int mixed_collection_test()
{
    struct collection_item *peer;
    struct collection_item *socket1;
    struct collection_item *socket2;
    struct collection_item *packet;
    struct collection_item *event;
    struct collection_item *host;
    char binary_dump[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    int found = 0;
    int class = 0;

    int error = EOK;

    TRACE_FLOW_STRING("mixed_collection_test","Entry.");

    printf("\n\nMIXED TEST!!!.\n\n\n");
    printf("Creating PEER collection.\n");

    if((error=create_collection(&peer,"peer",0)) ||
       (error=add_str_property(peer,NULL,"hostname","peerhost.mytest.com",0)) ||
       (error=add_str_property(peer,NULL,"IPv4","10.10.10.10",12)) || /* Expect trailing zero to be truncated */
       (error=add_str_property(peer,NULL,"IPv6","bla:bla:bla:bla:bla:bla",0))) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(peer);
        return error;
    }

    debug_collection(peer,COL_TRAVERSE_DEFAULT);

    printf("Creating HOST collection.\n");

    if((error=create_collection(&host,"host",0)) ||
       (error=add_str_property(host,NULL,"hostname","myhost.mytest.com",0)) ||
       (error=add_str_property(host,NULL,"IPv4","20.20.20.20",13)) ||
       (error=add_str_property(host,NULL,"IPv6","bla:bla:bla:bla:bla:bla",0)) ||
       (error=add_double_property(host,NULL,"double",0.253545))) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(peer);
        destroy_collection(host);
        return error;
    }

    debug_collection(host,COL_TRAVERSE_DEFAULT);

    printf("Creating SOCKET1 collection.\n");

    if((error=create_collection(&socket1,"socket1",0)) ||
       (error=add_int_property(socket1,NULL,"id",1)) ||
       (error=add_long_property(socket1,NULL,"packets",100000000L)) ||
       (error=add_binary_property(socket1,NULL,"stack",binary_dump,sizeof(binary_dump)))) {
        destroy_collection(peer);
        destroy_collection(host);
        destroy_collection(socket1);
        printf("Failed to add property. Error %d\n",error);
        return error;
    }

    debug_collection(socket1,COL_TRAVERSE_DEFAULT);
    printf("Creating a copy of SOCKET1 collection named SOCKET2.\n");

    error = copy_collection(&socket2,socket1,"socket2");
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        destroy_collection(socket1);
        printf("Failed to copy collection. Error %d\n",error);
        return error;
    }

    debug_collection(socket2,COL_TRAVERSE_DEFAULT);
    debug_collection(peer,COL_TRAVERSE_DEFAULT);

    printf("Adding PEER collection to SOCKET2 collection as a reference named PEER2\n");

    /* Embed peer host into the socket2 as reference */
    error = add_collection_to_collection(socket2,NULL,"peer2",peer,COL_ADD_MODE_REFERENCE);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        destroy_collection(socket1);
        destroy_collection(socket2);
        printf("Failed to create collection. Error %d\n",error);
        return error;
    }

    debug_collection(socket2,COL_TRAVERSE_DEFAULT);

    printf("Creating an EVENT collection.\n");

    /* Construct event */
    error = create_collection(&event,"event",0);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        destroy_collection(socket1);
        destroy_collection(socket2);
        printf("Failed to create collection. Error %d\n",error);
        return error;
    }

    debug_collection(event,COL_TRAVERSE_DEFAULT);

    printf("Adding HOST to EVENT.\n");

    /* Add host to event */
    error = add_collection_to_collection(event,NULL,NULL,host,COL_ADD_MODE_REFERENCE);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        destroy_collection(socket1);
        destroy_collection(socket2);
        printf("Failed to add collections. Error %d\n",error);
        return error;
    }

    debug_collection(event,COL_TRAVERSE_DEFAULT);

    printf("Embed SOCKET1 into EVENT.\n");
    /* Donate socket1 to event */
    /* Socket1 should not be used after this */
    error = add_collection_to_collection(event,NULL,NULL,socket1,COL_ADD_MODE_EMBED);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        destroy_collection(socket1);
        destroy_collection(socket2);
        printf("Failed to add collections. Error %d\n",error);
        return error;
    }

    printf("Traverse one level:\n");
    debug_collection(event,COL_TRAVERSE_ONELEVEL);
    printf("Traverse ignore subcollections:\n");
    debug_collection(event,COL_TRAVERSE_IGNORE);
    printf("Traverse normal:\n");
    debug_collection(event,COL_TRAVERSE_DEFAULT);
    debug_collection(socket1,COL_TRAVERSE_DEFAULT);

    printf("SOCKET1 MUST NO BE USED AFTER THIS POINT!!!\n");
    socket1 = (struct collection_item *)(NULL);

    printf("Add collection PEER as PEER1 to subcollection SOCKET1 of the EVENT.\n");

    debug_collection(peer,COL_TRAVERSE_DEFAULT);

    error = add_collection_to_collection(event,"socket1","peer1",peer,COL_ADD_MODE_CLONE);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        /* No socket1 any more :) */
        destroy_collection(socket2);
        printf("Failed to add collections. Error %d\n",error);
        return error;
    }

    debug_collection(event,COL_TRAVERSE_DEFAULT);

    printf("Add property named TIMEOUT to PEER collection.\n");

    /* Add new property to the peer collection */
    error = add_int_property(peer,NULL,"timeout",5);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        /* No socket1 any more :) */
        destroy_collection(socket2);
        printf("Failed to add property. Error %d\n",error);
        return error;
    }

    debug_collection(socket2,COL_TRAVERSE_DEFAULT);

    printf("Add property named DELAY to PEER1 collection.\n");

    error = add_int_property(event,"peer1","delay",10);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        /* No socket1 any more :) */
        destroy_collection(socket2);
        printf("Failed to add property. Error %d\n",error);
        return error;
    }

    debug_collection(event,COL_TRAVERSE_DEFAULT);
    debug_collection(host,COL_TRAVERSE_DEFAULT);

    printf("Check if property PEER1.DELAY is in the EVENT collection.\n");

    /* Check if the property in the collection */
    found = 0;
    error = is_item_in_collection(event, "peer1.delay", COL_TYPE_ANY, COL_TRAVERSE_DEFAULT, &found);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        /* No socket1 any more :) */
        destroy_collection(socket2);
        printf("Failed to check property. Error %d\n",error);
        return error;
    }

    if (found == 1) printf("Property is found!\n");
    else printf("Error property is not found!\n");


    print_item(event, "peer1.IPv6");
    print_item(event, "event.socket1.peer1.IPv6");
    print_item(event, "event.peer1.IPv6");
    print_item(event, "speer1.IPv6");
    print_item(event, "eer1.IPv6");
    print_item(event, ".peer1.IPv6");
    print_item(event, "t.peer1.IPv6");

    /* Traverse collection */
    error = print_collection2(event);
    if(error) {
        printf("Error printing collection %d\n",error);
        return error;
    }

    printf("Delete property PEER1.DELAY from the EVENT collection.\n");

    error = delete_property(event,"peer1.delay",COL_TYPE_ANY, COL_TRAVERSE_DEFAULT);
    if(error) {
        destroy_collection(peer);
        destroy_collection(host);
        /* No socket1 any more :) */
        destroy_collection(socket2);
        printf("Failed to delete property. Error %d\n",error);
        return error;
    }

    printf("Printing EVENT.\n");

    /* Traverse collection */
    error = print_collection2(event);
    if(error) {
        printf("Error printing collection %d\n",error);
        return error;
    }
    printf("Debugging EVENT.\n");

    error = debug_collection(event,COL_TRAVERSE_DEFAULT);
    if(error) {
        printf("Error printing collection %d\n",error);
        return error;
    }

    printf("Cleanup of the collections PEER, HOST and SOCKET2.\n");

    /* Destroy a referenced collection */
    destroy_collection(peer);
    destroy_collection(host);
    destroy_collection(socket2);

    printf("Printing EVENT again.\n");

    /* Traverse collection again - peer should still be there */
    error = print_collection(event);
    if(error) {
        printf("Error printing collection %d\n",error);
        return error;
    }

    printf("Debugging EVENT again.\n");

    error = debug_collection(event,COL_TRAVERSE_DEFAULT);
    if(error) {
        printf("Error printing collection %d\n",error);
        return error;
    }

    printf("Attempt to add property to a referenced collection.\n");

    /* Some negative tests */
    /* Can't add attributes to the referenced collection */
    error = add_int_property(event,"host","session",500);
    if(error != 0) printf("Error was NOT able to add property to a referenced collection.\n");
    else {
        printf("Unexpected success which is an implementation error.\n");
        return error;
    }

    printf("Attempt to delete non-existent property.\n");

    /* Can't delete non exitent property */
    error = delete_property(event,"host.host",COL_TYPE_ANY, COL_TRAVERSE_DEFAULT);
    if(error == 0) {
        printf("Error was able to delete property that does not exist.\n");
        return -1;
    }
    else printf("Expected error %d\n",error);

    /* Set collection class */
    error = set_collection_class(event,2);
    if(error != 0) {
        printf("Error was NOT able to set class.\n");
        return error;
    }

    error = get_collection_class(event,&class);
    if(error != 0) {
        printf("Error was NOT able to get class.\n");
        return error;
    }
    else printf("Class = %d\n",class);

    if(is_of_class(event,2)) printf("Class mathced!\n");
    else {
        printf("Error - bad class.\n");
        return -1;
    }

    printf("Done. Cleaning...\n");

    destroy_collection(event);

    printf("Exit.\n");
    TRACE_FLOW_NUMBER("add_collection_test. Returning",EOK);
    return EOK;
}


int iterator_test()
{
    struct collection_item *peer;
    struct collection_item *socket1;
    struct collection_item *socket2;
    struct collection_iterator *iterator = (struct collection_iterator *)(NULL);
    int error = EOK;
    struct collection_item *item = (struct collection_item *)(NULL);
    char binary_dump[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    int depth = 0;

    printf("\n\n==== ITERATOR TEST ====\n\n");

    if((error=create_collection(&peer,"peer",0)) ||
       (error=add_str_property(peer,NULL,"hostname","peerhost.mytest.com",0)) ||
       (error=add_str_property(peer,NULL,"IPv4","10.10.10.10",12)) || /* Expect trailing zero to be truncated */
       (error=add_str_property(peer,NULL,"IPv6","bla:bla:bla:bla:bla:bla",0))) {
        printf("Failed to add property. Error %d",error);
        destroy_collection(peer);
        return error;
    }

    if((error=create_collection(&socket1,"socket",0)) ||
       (error=add_int_property(socket1,NULL,"id",1)) ||
       (error=add_long_property(socket1,NULL,"packets",100000000L)) ||
       (error=add_binary_property(socket1,NULL,"stack",binary_dump,sizeof(binary_dump)))) {
        destroy_collection(peer);
        destroy_collection(socket1);
        printf("Failed to add property. Error %d\n",error);
        return error;
    }

    if((error=create_collection(&socket2,"socket",0)) ||
       (error=add_int_property(socket2,NULL,"id",2)) ||
       (error=add_long_property(socket2,NULL,"packets",200000000L)) ||
       (error=add_binary_property(socket2,NULL,"queue",binary_dump,sizeof(binary_dump)))) {
        destroy_collection(peer);
        destroy_collection(socket1);
        destroy_collection(socket2);
        printf("Failed to add property. Error %d\n",error);
        return error;
    }

    error = add_collection_to_collection(peer,NULL,"first",socket1,COL_ADD_MODE_EMBED);
    if(error) {
        destroy_collection(peer);
        destroy_collection(socket1);
        destroy_collection(socket2);
        printf("Failed to add collection to collection. Error %d\n",error);
        return error;
    }

    error = add_collection_to_collection(peer,NULL,"second",socket2,COL_ADD_MODE_EMBED);
    if(error) {
        destroy_collection(peer);
        destroy_collection(socket2);
        printf("Failed to add collection to collection. Error %d\n",error);
        return error;
    }

    /* Bind iterator */
    error =  bind_iterator(&iterator,peer,COL_TRAVERSE_DEFAULT);
    if(error) {
        printf("Error (bind): %d\n",error);
        destroy_collection(peer);
        return error;
    }

    printf("\n\nCollection:\n\n");
    debug_collection(peer,COL_TRAVERSE_DEFAULT);

    /* This should also work becuase iterator holds to collection */
    destroy_collection(peer);

    printf("\n\nIteration:\n\n");

    do {
        depth = 0;
        get_iterator_depth(iterator,&depth);

        /* Loop through a collection */
        error = iterate_collection(iterator, &item);
        if(error) {
            printf("Error (iterate): %d\n",error);
            unbind_iterator(iterator);
            return error;
        }

        /* Are we done ? */
        if(item == (struct collection_item *)(NULL)) break;

        printf("%*s Property (%s), type = %d, data size = %d\n",
                depth * 4,  "",
                get_item_property(item,NULL),
                get_item_type(item),
                get_item_length(item));

        if((strcmp(get_item_property(item,NULL),"id")==0) &&
           (*((int *)(get_item_data(item))) == 1)) {
            printf("\n\nFound property we need - go up!!!\n\n\n");
            error = iterate_up(iterator,5);
            if(!error) {
                printf("We expected error but got seucces - bad.\n");
                unbind_iterator(iterator);
                return -1;
            }
            /* This should work! */
            error = iterate_up(iterator,1);
            if(error) {
                printf("We expected success but got error %d\n",error);
                unbind_iterator(iterator);
                return error;
            }

            if((error = modify_str_item(item,"id2","test",0)) ||
               (error = debug_item(item)) ||
               (error = modify_str_item(item,NULL,"test",2)) ||
               (error = debug_item(item)) ||
               (error = modify_binary_item(item,NULL,binary_dump,sizeof(binary_dump))) ||
               (error = debug_item(item)) ||
               (error = modify_bool_item(item,NULL,1)) ||
               (error = debug_item(item)) ||
               (error = modify_int_item(item,"int",1)) ||
               (error = debug_item(item)) ||
               (error = modify_long_item(item,"long",1000000000L)) ||
               (error = debug_item(item)) ||
               (error = modify_ulong_item(item,"ulong",4000000000UL)) ||
               (error = debug_item(item)) ||
               (error = modify_unsigned_item(item,"unsigned",4000000000U)) ||
               (error = debug_item(item)) ||
               (error = modify_double_item(item,"double",-1.1)) ||
               (error = debug_item(item))) {
                printf("Failed to change property.\n");
                return error;
            }
        }
    }
    while(1);

    /* Do not forget to unbind iterator - otherwise there will be a leak */
    unbind_iterator(iterator);
    return EOK;
}


/* Main function of the unit test */

int main()
{
    int error = 0;

    printf("Start\n");
    if((error=ref_collection_test()) ||
       (error=single_collection_test()) ||
       (error=add_collection_test()) ||
       (error=mixed_collection_test()) ||
       (error=iterator_test())) {
        printf("Failed!\n");
    }
    else printf("Success!\n");
    /* Add other tests here ... */
    return error;
}


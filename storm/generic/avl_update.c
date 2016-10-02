// Abstract: Hifi-Eslöf specific functions.
// Author: Henrik Hallin <hal@chaosdev.org>
//         Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000, 2013, 2016 chaos development.

// Define as TRUE to get lots of debug information.
#define DEBUG FALSE

#include <storm/generic/avl.h>
#include <storm/generic/avl_debug.h>
#include <storm/generic/avl_update.h>
#include <storm/generic/defines.h>
#include <storm/generic/debug.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/memory_physical.h>

// Update the hifi-eslöf specific largest_free*-fields. This function returns TRUE if the node needed to be update, or FALSE
// otherwise.
bool avl_update_node_largest_free(avl_node_type *node)
{
    char debug_string[MAX_LOG_MESSAGE_LENGTH];
    unsigned int largest_free_less, largest_free_more;

    memory_copy(debug_string, __FUNCTION__, MAX_LOG_MESSAGE_LENGTH);
    avl_debug_log_write(debug_string);

    DEBUG_MESSAGE(DEBUG, "Called on node (%u, %u, %u).", node->start, node->busy_length, node->free_length);

    if (node == NULL)
    {
        DEBUG_HALT("node == NULL");
    }

    largest_free_less = node->largest_free_less;
    largest_free_more = node->largest_free_more;

    if (node->less != NULL)
    {
        node->largest_free_less = MAX_OF_THREE(node->less->largest_free_less,
                                               node->less->largest_free_more,
                                               node->less->free_length);
    }
    else
    {
        node->largest_free_less = 0;
    }

    if (node->more != NULL)
    {
        node->largest_free_more = MAX_OF_THREE(node->more->largest_free_less,
                                               node->more->largest_free_more,
                                               node->more->free_length);
    }
    else
    {
        node->largest_free_more = 0;
    }

    //  debug_print ("old %u, new %u\n", largest_free_more, node->largest_free_more);

    // If the largest free values didn't change, we don't have to update the tree any further.
    if (node->largest_free_less == largest_free_less &&
        node->largest_free_more == largest_free_more)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

// This function loops up a tree and updates all ancestors as long as they need to be updated.
void avl_update_tree_largest_free(avl_node_type *tree)
{
    char debug_string[MAX_LOG_MESSAGE_LENGTH];

    memory_copy(debug_string, __FUNCTION__, MAX_LOG_MESSAGE_LENGTH);
    avl_debug_log_write(debug_string);

    DEBUG_MESSAGE(DEBUG, "Entering.");

    while (tree != NULL && avl_update_node_largest_free(tree))
    {
        tree = (avl_node_type *) tree->parent;
    }

    DEBUG_MESSAGE(DEBUG, "Leaving.");
}

// Check if the tree is to grow.
void avl_update_mappings(avl_header_type *avl_header)
{
    char debug_string[MAX_LOG_MESSAGE_LENGTH];

    bool growable = avl_header->growable;

    memory_copy(debug_string, __FUNCTION__, MAX_LOG_MESSAGE_LENGTH);
    avl_debug_log_write(debug_string);

    // Is it time to let the bitmap grow?
    if (avl_header->growable &&
        ((avl_header->number_of_nodes / 8) + sizeof (avl_header_type)) >
        (avl_header->pages_allocated_bitmap * SIZE_PAGE - 1) &&
        avl_header->limit_pages_bitmap > avl_header->pages_allocated_bitmap)
    {
        uint32_t new_page;

        // FIXME: It is uneccessary to clear this flag for all AVL-trees, find out a smart way of doing this.
        // This flag is set to FALSE, or else this will recurse unpredicable.
        avl_header->growable = FALSE;

        //    debug_print ("Bitmap is growing...");
        //    avl_dump_header (avl_header);

        // FIXME: Check return value.
        memory_physical_allocate(&new_page, 1, "AVL bitmap");

        memory_virtual_map(GET_PAGE_NUMBER((uint32_t) avl_header) + avl_header->pages_allocated_bitmap, new_page, 1, PAGE_KERNEL);

        avl_header->pages_allocated_bitmap++;
    }

    // Now, check if the node array needs to grow.
    if (avl_header->growable &&
        (avl_header->number_of_nodes * sizeof (avl_node_type)) >
        ((avl_header->pages_allocated_array * SIZE_PAGE) -
         (4 * sizeof (avl_node_type))) &&
        avl_header->limit_pages_array > avl_header->pages_allocated_array)
    {
        uint32_t new_page;

        // This flag is set to FALSE, or else this will recurse unpredicable.

        //    debug_print ("Nodearray at 0x%x is growing...",
        //                 (uint32_t) avl_header->node_array);
        //    avl_dump_header (avl_header);

        avl_header->growable = FALSE;

        // FIXME: Check return value.
        memory_physical_allocate(&new_page, 1, "AVL node array");

        memory_virtual_map(GET_PAGE_NUMBER((uint32_t) avl_header->node_array) + avl_header->pages_allocated_array, new_page, 1, PAGE_KERNEL);

        avl_header->pages_allocated_array++;
    }

    // It is now safe for the structures to grow again.
    avl_header->growable = growable;
}

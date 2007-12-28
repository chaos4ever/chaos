/* $Id$ */
/* Author: Anders Ohrt <doa@chaosdev.org> */
/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#include <storm/generic/avl.h>
#include <storm/generic/avl_delete.h>
#include <storm/generic/return_values.h>
#include <storm/generic/types.h>
#include <storm/generic/defines.h>
#include <storm/generic/debug.h>

/* FIXME: There are lots of nasties in this code, since it hasn't been
   completely converted from 'math' to C yet (the 'math' version hade
   tons of goto's and stuff). */

#define AUX_ARRAY_LENGTH 64

#define LARGEST_CHILD(key, P) (key == +1 ? P->more : P->less)

#define avl_copy_content(a, b) {}

static void rebalance (void);
static void single_rotation_with_balanced_child (avl_node_type *node,
                                                 avl_node_type *child);
static void single_rotation_with_unbalanced_child (avl_node_type *node,
                                                 avl_node_type *child);
static void double_rotation (avl_node_type *node, avl_node_type *child);

static int a[AUX_ARRAY_LENGTH];
static avl_node_type *P[AUX_ARRAY_LENGTH];
static int depth;

return_type avl_key_delete (avl_header_type *header,
                            unsigned int key)
{
  /* Fixme: Mutex this! */

  /*
    node->start = start;
    node->busy_length = busy_length;
    node->free_length = free_length;
    node->parent = (struct avl_node_type *) parent;
    node->less = NULL;
    node->more = NULL;
    node->balance = 0;
    node->largest_free_less = 0;
    node->largest_free_more = 0;
  */

  bool found_node = FALSE;

  int l;
  avl_node_type *node;
  avl_node_type *parent;
  avl_node_type *R;
  avl_node_type *S;

  /* Initialize. */

  a[0] = -1;
  P[0] = NULL;
  depth = 1;
  node = header->root;
  parent = NULL;

  while (found_node == FALSE && node != NULL)
  {
    P[depth] = node;

    if (key < node->start)
    {
      a[depth] = -1;
      parent = node;
      node = node->less;
    }
    else if (key > node->start)
    {
      a[depth] = +1;
      parent = node;
      node = node->more;
    }
    else
    {
      found_node = TRUE;
    }
    depth++;
  }

  if (node == NULL)
  {
    DEBUG_HALT ("Key wasn't in the tree!");
  }

  /* If the more node is NULL, we can move the less node one step up,
     and we're done. */

  if (node->more == NULL)
  {
    if (parent == NULL)
    {
      /* Fixme: free the node, and let header point to node->less. */
    }
    else
    {
      avl_copy_content (parent, node->less);
      parent->balance = 0;
    }
    rebalance ();
    return RETURN_SUCCESS;
  }

  /* Find successor. */

  R = node->more;

  if (R->less == NULL)
  {
    R->less = node->less;
    avl_copy_content (parent, R);
    R->balance = node->balance;
    a[depth] = +1;
    P[depth] = R;
    depth++;      

    /* Fixme: free the node. */

    rebalance ();
    return RETURN_SUCCESS;
  }

  /* Set up to find NULL less child. */

  S = R->less;
  l = depth;
  depth++;
  a[depth] = -1;
  P[depth] = R;
  depth++;


  /* Find NULL less child. */

  while (S->less != NULL)
  {
    R = S;
    S = R->less;
    a[depth] = -1;
    P[depth] = R;
    depth++;
  }

  /* Fix up. */

  a[l] = +1;
  P[l] = S;

  S->less = node->less;
  R->less = S->more;
  S->more = node->more;
  S->balance = node->balance;

  avl_copy_content (parent, S);
  rebalance ();

  return RETURN_SUCCESS;
}


static void rebalance (void)
{
  avl_node_type *node;
  avl_node_type *child;

  for (;depth > 0; depth--)
  {
    node = P[depth];

    if (node->balance == 0)
    {
      node->balance = -a[depth];
      return;
    }
    else if (node->balance == a[depth])
    {
      node->balance = 0;
      continue;
    }
    else /* node->balance == -a[depth]. */
    {
      child = LARGEST_CHILD (-a[depth], node);

      if (child->balance == 0)
      {
         /* Single rotation with balanced child. */

        single_rotation_with_balanced_child (node, child);

        /* In this case, the tree will be fully balanced, so we
           return. */

        return;
      }
      else if (child->balance == -a[depth])
      {
        /* Single rotation with unbalanced child. */

        single_rotation_with_unbalanced_child (node, child);

        /* In this case, the tree will not be balance, so we continue
           up the tree. */

        continue;
      }
      else /* child->balance == a[depth]. */
      {
        /* Double rotation. */

        double_rotation (node, child);

        /* In this case, the tree will not be balance, so we continue
           up the tree. */

        continue;
      }
    }
  }
  return;
}

#define LARGEST_CHILD(key, P) (key == +1 ? P->more : P->less)

static inline avl_node_type *largest_child(int key, avl_node_type *node)
{
  if (key == 1)
  {
    return node->more;
  }
  else
  {
    return node->less;
  }  
}

static void update_largest_child(int key, avl_node_type *node, 
                                 avl_node_type *value)
{
  if (key == +1) 
  { 
    node->more = value;
  }
  else
  {
    node->less = value;
  }
 
}

static void single_rotation_with_balanced_child (avl_node_type *node,
                                                 avl_node_type *child)
{
  /* Single rotation with balanced child. */
  update_largest_child(-a[depth], node, LARGEST_CHILD(a[depth], child));
  update_largest_child(a[depth], child, node);
  child->balance = a[depth];
  update_largest_child(a[depth - 1], P[depth - 1], child);
}

static void single_rotation_with_unbalanced_child (avl_node_type *node,
                                                   avl_node_type *child)
{
  /* Single rotation with unbalanced child. */

  update_largest_child(-a[depth], node, LARGEST_CHILD (a[depth], child));
  update_largest_child(a[depth], child,  node);
  node->balance = child->balance = 0;
  update_largest_child(a[depth-1], P[depth-1], child);
}

static void double_rotation (avl_node_type *node, avl_node_type *child)
{
  /* Double rotation. */

  node = LARGEST_CHILD (a[depth], child);
  update_largest_child(a[depth], child, LARGEST_CHILD (-a[depth], node));
  update_largest_child(-a[depth], node, child);
  update_largest_child(-a[depth], node, LARGEST_CHILD (a[depth], node));
  update_largest_child(a[depth], node, node);

  if (node->balance == -a[depth])
  {
    node->balance = a[depth];
    child->balance = 0;
  }
  else if (node->balance == 0)
  {
    node->balance = child->balance = 0;
  }
  else /* node->balance = a[depth]; */
  {
    node->balance = 0;
    child->balance = -a[depth];
  }

  node->balance = 0;
  update_largest_child(a[depth-1], P[depth-1], node);
}

/* $Id$ */
/* Abstract: Connection handling in the land of Oz. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#include "config.h"
#include "console.h"
#include "console_output.h"

static void connection_client
  (message_parameter_type *message_parameter, console_type **our_console, 
   console_application_type **our_application, u32 *data, 
   ipc_structure_type *ipc_structure)
{
  switch (message_parameter->message_class)
  {
    /* Output text to this console, no matter what mode we are
       in. */
    
    case IPC_CONSOLE_OUTPUT:
    {
      if (*our_console != NULL && has_video)
      {
        /* This is not purely correct, but it's better than
           nothing... */
        /* FIXME: library_semaphore. */
        
        while ((*our_console)->lock == TRUE)
        {
          system_call_dispatch_next ();
        }
        
        (*our_console)->lock = TRUE;
        console_output (*our_console, message_parameter->data);
        (*our_console)->lock = FALSE;
      }
      
      break;
    }
    
    case IPC_CONSOLE_KEYBOARD_EXTENDED:
    {
      (*our_application)->keyboard_type = CONSOLE_KEYBOARD_EXTENDED;
      break;
    }
    
    case IPC_CONSOLE_KEYBOARD_NORMAL:
    {
      (*our_application)->keyboard_type = CONSOLE_KEYBOARD_NORMAL;
      break;
    }
    
    case IPC_CONSOLE_ENABLE_KEYBOARD:
    {
      (*our_application)->wants_keyboard = TRUE;
      break;
    }
    
    case IPC_CONSOLE_DISABLE_KEYBOARD:
    {
      (*our_application)->wants_keyboard = FALSE;
      break;
    }
    
    case IPC_CONSOLE_ENABLE_MOUSE:
    {
      (*our_application)->wants_mouse = TRUE;
      break;
    }
    
    case IPC_CONSOLE_DISABLE_MOUSE:
    {
      (*our_application)->wants_mouse = FALSE;
      break;
    }
    
    /* Open a new console with the requested attributes. */
    
    case IPC_CONSOLE_OPEN:
    {
      ipc_console_attribute_type *console_attribute =
        (ipc_console_attribute_type *) data;
      video_mode_type video_mode;
      
      if (*our_console == NULL)
      {
        memory_allocate ((void **) our_console, sizeof (console_type));
        
        /* Fill in some required stuff. */
        
        (*our_console)->width = console_attribute->width;
        (*our_console)->height = console_attribute->height;
        (*our_console)->depth = console_attribute->depth;
        (*our_console)->type = console_attribute->mode_type;
        
        (*our_console)->cursor_x = 0;
        (*our_console)->cursor_y = 0;
        
        (*our_console)->cursor_saved_x = -1;
        (*our_console)->cursor_saved_y = -1;
        
        (*our_console)->state = CONSOLE_STATE_CHARACTER;
        (*our_console)->numeric_argument_index = 0;
        
        (*our_console)->current_attribute = CONSOLE_DEFAULT_ATTRIBUTE;
        
        /* Allocate memory for a buffer for this console. */
        
        character_type **buffer_pointer = &((*our_console)->buffer);
        if ((*our_console)->type == VIDEO_MODE_TYPE_TEXT)
        {
          memory_allocate ((void **) buffer_pointer,
                           (*our_console)->width *
                           (*our_console)->height *
                           sizeof (character_type));
        }
        else
        {
          memory_allocate ((void **) buffer_pointer,
                           (*our_console)->width *
                           (*our_console)->height *
                           (*our_console)->depth);
        }
        
        (*our_console)->output = (*our_console)->buffer;
        (*our_console)->lock = FALSE;
        
        (*our_console)->application_list = *our_application;
        (*our_console)->active_application = *our_application;
        
        (*our_application)->next = NULL;
        
        (*our_application)->ipc_structure.input_mailbox_id = 
          ipc_structure->input_mailbox_id;
        (*our_application)->ipc_structure.output_mailbox_id =
          ipc_structure->output_mailbox_id;
        
        /* Is this the first console? If so, activate it. */
        
        if (current_console == NULL)
        {
          current_console = *our_console;
          (*our_console)->output = screen;
          
          if (has_video)
          {
            /* Try to set the requested video mode. */
            /* FIXME: Error handling and return values! */
            
            video_mode.width = console_attribute->width;
            video_mode.height = console_attribute->height;
            video_mode.depth = console_attribute->depth;
            video_mode.mode_type = console_attribute->mode_type;
            
            if (video_mode_set (&video_structure,
                                &video_mode) != VIDEO_RETURN_SUCCESS)
            {
              /* FIXME: Fail and return here. */
            }
          }
        }
        
        console_link (*our_console);
        
        /* We have added a new console. */
        
        number_of_consoles++;
      }
      
      break;
    }
    
    /* Change the video mode for this console. */
    
    case IPC_CONSOLE_MODE_SET:
    {
      video_mode_type video_mode;
      ipc_console_attribute_type *console_attribute =
        (ipc_console_attribute_type *) data;
      
      video_mode.width = console_attribute->width;
      video_mode.height = console_attribute->height;
      video_mode.depth = console_attribute->depth;
      video_mode.mode_type = console_attribute->mode_type;
      
      if (video_mode_set (&video_structure, 
                          &video_mode) != VIDEO_RETURN_SUCCESS)
      {
        /* FIXME: Fail and return here. */
      }
      else
      {
        (*our_console)->width = console_attribute->width;
        (*our_console)->height = console_attribute->height;
        (*our_console)->depth = console_attribute->depth;
        (*our_console)->type = console_attribute->mode_type;
        
        (*our_console)->cursor_x = 0;
        (*our_console)->cursor_y = 0;
        
        (*our_console)->cursor_saved_x = -1;
        (*our_console)->cursor_saved_y = -1;
      }
      
      break;
    }
    
    /* Resize the console (character rows and columns), but do nothing
       to the mode of the console. */
    
    case IPC_CONSOLE_RESIZE:
    {
#if FALSE
      /* FIXME: Most of this is wrong. */
      
      video_mode_type video_mode;
      ipc_console_resize_type *ipc_console_resize =
        (ipc_console_resize_type *) data;
      
      if (our_console != NULL)
      {
        video_mode.width = ipc_console_resize->width;
        video_mode.height = ipc_console_resize->height;
        video_mode.depth = 4;
        video_mode.mode_type = VIDEO_MODE_TYPE_TEXT;
        
        if (video_mode_set (&video_mode) == VIDEO_RETURN_SUCCESS)
        {
          our_console->width = ipc_console_resize->width;
          our_console->height = ipc_console_resize->height;
        }            
        
        /* FIXME: Return something. */
      }
#endif
      break;
    }
  }
}

static void connection_provider_keyboard
  (message_parameter_type *message_parameter)
{
  /* Console switching? For now, ALT + TAB is used. This should be
     customisable. */
        
  switch (message_parameter->message_class)
  {
    case IPC_CONSOLE_KEYBOARD_EVENT:
    {
      keyboard_packet_type *keyboard_packet = (keyboard_packet_type *)
        message_parameter->data;
            
      if (current_console != NULL)
      {
        if (keyboard_packet->key_pressed &&
            keyboard_packet->has_special_key &&
            keyboard_packet->special_key == IPC_KEYBOARD_SPECIAL_KEY_TAB &&
            (keyboard_packet->left_control_down ||
             keyboard_packet->right_control_down))
        {
          /* Next console. */
                
          console_type *new_console = (console_type *) current_console->next;
          if (new_console == NULL)
          {
            new_console = console_list;
          }
                
          if (new_console != current_console)
          {
            /* FIXME: This is not purely correct, but what the
               heck... finish library_semaphore sometime, will
               you? */
            
            while (new_console->lock == TRUE)
            {
              system_call_dispatch_next ();
            }
            
            new_console->lock = TRUE;
            console_flip (new_console);
            new_console->lock = FALSE;
          }
        }
              
        /* Bind a console to a function key. */
              
        if (keyboard_packet->key_pressed &&
            keyboard_packet->has_special_key &&
            keyboard_packet->special_key >= IPC_KEYBOARD_SPECIAL_KEY_F1 &&
            keyboard_packet->special_key <= IPC_KEYBOARD_SPECIAL_KEY_F10 &&
            (keyboard_packet->left_alt_down ||
             keyboard_packet->right_alt_down) &&
            (keyboard_packet->left_control_down ||
             keyboard_packet->right_control_down))
        {
          console_shortcut[keyboard_packet->special_key -
                          IPC_KEYBOARD_SPECIAL_KEY_F1] = current_console;
        }
              
        /* Switch to a previously bound console. */
              
        if (keyboard_packet->key_pressed &&
            keyboard_packet->has_special_key &&
            keyboard_packet->special_key >= IPC_KEYBOARD_SPECIAL_KEY_F1 &&
            keyboard_packet->special_key <= IPC_KEYBOARD_SPECIAL_KEY_F10 &&
            (keyboard_packet->left_alt_down ||
             keyboard_packet->right_alt_down))
        {
          /* New console. */
                
          console_type *new_console =
            (console_type *)console_shortcut[keyboard_packet->special_key -
                                            IPC_KEYBOARD_SPECIAL_KEY_F1];
          if (new_console != NULL)
          {
            /* FIXME: This is not purely correct, but what the
               heck... finish library_semaphore sometime, will you? */
                  
            while (new_console->lock == TRUE)
            {
              system_call_dispatch_next ();
            }
                  
            new_console->lock = TRUE;
            console_flip (new_console);
            new_console->lock = FALSE;
          }
        }
              
        /* Send on... If we are allowed to. */
              
        if (current_console->active_application != NULL)
        {
          if (current_console->active_application->wants_keyboard)
          {
            message_parameter->block = FALSE;
            message_parameter->protocol = IPC_PROTOCOL_CONSOLE;
            message_parameter->message_class = IPC_CONSOLE_KEYBOARD_EVENT;
            message_parameter->length = sizeof (keyboard_packet_type);
            ipc_send (current_console->active_application->ipc_structure.output_mailbox_id,
                      message_parameter);
          }
        }
        else if (current_console->active_application == NULL)
        {
          // system_call_debug_print_simple ("console: Something is broken. A mutex will fix this!\n");
        }
      }        
    }
                
    default:
    {
      break;
    }
  }
}

static void connection_provider_mouse
  (message_parameter_type *message_parameter)
{
  switch (message_parameter->message_class)
  {
    case IPC_CONSOLE_MOUSE_EVENT:
    {
      if (current_console->active_application != NULL)
      {
        if (current_console->active_application->wants_mouse)
        {
          message_parameter->block = FALSE;
          message_parameter->protocol = IPC_PROTOCOL_CONSOLE;
          message_parameter->message_class = IPC_CONSOLE_MOUSE_EVENT;
          message_parameter->length = sizeof (ipc_mouse_event_type);
          ipc_send (current_console->active_application->ipc_structure.output_mailbox_id,
                    message_parameter);
        }
      }
    }
    
    default:
    {
      break;
    }
  }
}

/* Handle an IPC connection request. */

void handle_connection (mailbox_id_type reply_mailbox_id)
{
  u32 *data;
  u32 **data_pointer = &data;
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  unsigned int connection_class = IPC_CONSOLE_CONNECTION_CLASS_NONE;
  console_type *our_console = NULL;
  console_application_type *our_application = NULL;
  console_application_type **our_application_pointer = &our_application;
  unsigned int data_size = 4096;

  memory_allocate ((void **) data_pointer, data_size);
  memory_allocate ((void **) our_application_pointer,
                   sizeof (console_application_type));

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);

  message_parameter.data = data;

  while (TRUE)
  {
    message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    message_parameter.block = TRUE;
  
    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter,
                     &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }
    
    switch (connection_class)
    {
      case IPC_CONSOLE_CONNECTION_CLASS_NONE:
      {
        if (message_parameter.message_class ==
            IPC_CONSOLE_CONNECTION_CLASS_SET)
        {
          connection_class = *((unsigned int *) message_parameter.data);

          if (connection_class == IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_VIDEO)
          {
            has_video = TRUE;

            video_structure.output_mailbox_id = 
              ipc_structure.output_mailbox_id;
            video_structure.input_mailbox_id =
              ipc_structure.input_mailbox_id;
          }
        }

        break;
      }

      case IPC_CONSOLE_CONNECTION_CLASS_CLIENT:
      {
        connection_client (&message_parameter, &our_console, &our_application,
                           data, &ipc_structure);
        break;
      }

      /* A keyboard provider has something for us. */
      
      case IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_KEYBOARD:
      {
        connection_provider_keyboard (&message_parameter);
        break;
      }
        
      case IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_MOUSE:
      {
        connection_provider_mouse (&message_parameter);
        break;
      }
      
      case IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_JOYSTICK:
      {
        break;
      }
      
      case IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_VIDEO:
      {
        //        connection_provider_video (&message_parameter);
        break;
      }

      break;
    }
  }
}


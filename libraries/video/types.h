// Abstract: Graphics library type definitions.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#pragma once

typedef struct
{
    // Video mode resolution.
    unsigned int width;
    unsigned int height;
    unsigned int depth;

    // Do we want a double buffered mode? We may get it anyway, but we might as well state what we'd prefer. For example, most
    // games would likely always want a double buffered mode for flicker-free animation.
    bool buffered;

    // Which type of mode do we want? (Graphic or text)
    unsigned int mode_type;
} video_mode_type;

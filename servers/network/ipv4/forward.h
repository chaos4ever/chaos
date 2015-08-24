// Abstract: Function prototypes for the packet forwarding code.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

// Function prototypes.
extern void forward_packet(ipv4_ethernet_header_type *ethernet_header, unsigned int length);

// Globals.
extern bool forwarding;

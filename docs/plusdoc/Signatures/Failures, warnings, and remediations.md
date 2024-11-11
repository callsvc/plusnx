#### W: (NCA signature verification failed) 
NSP and XCI have most of their content encrypted in a layer called NCA. The first bytes of this layer (0x0 - 0x200) represent RSA 2048 digital signatures of the following bytes in the internal header (from offset 0x200 to 0x400). However, this signature might not match if the NSP in question results from a direct XCI conversion, as some header flags are modified in the conversion process


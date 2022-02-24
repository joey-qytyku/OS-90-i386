#ifndef OWINTRIN_H
#define OWINTRIN_H

 #pragma intrinsic(inp,inpw,outp,outpw,inpd,outpd)

 _WCIRTLINK extern unsigned inp(unsigned __port);
 _WCIRTLINK extern unsigned inpw(unsigned __port);
 _WCIRTLINK extern unsigned outp(unsigned __port, unsigned __value);
 _WCIRTLINK extern unsigned outpw(unsigned __port,unsigned __value);

#define PORT_T unsigned char

#define byte unsigned char

/* Check if interrupts enabled? */

#endif

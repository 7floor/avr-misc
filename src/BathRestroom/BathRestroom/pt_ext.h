/*
 * pt_ext.h
 *
 * Created: 05.03.2014 0:33:02
 *  Author: Dmitry
 */ 

#ifndef PT_EXT_H_
#define PT_EXT_H_

#include <stddef.h>
//#define LC_INCLUDE "lc-addrlabels.h"
#include "pt/pt.h"

#define PT_ENDLESS(pt) LC_END((pt)->lc); return 0; }

#endif /* PT_EXT_H_ */
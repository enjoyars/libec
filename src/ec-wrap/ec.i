%module ec

%include "carrays.i"
%array_class(int, intArray);

%{
#include "ec.h"
%}

%include "ec.h"

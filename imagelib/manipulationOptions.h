//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_MANIPULATIONS_MANIPULATIONOPTIONS_H
#define BACKEND_MANIPULATIONS_MANIPULATIONOPTIONS_H

//Projectwide includes
#include "sprocessstatuswidget.h"

class ManipulationOptions 
{ 
public:
  ManipulationOptions( SProcessStatusWidget* status ) { this->status = status; }
  SProcessStatusWidget* getStatus() { return status; }
private:
  SProcessStatusWidget* status;
};

#endif //BACKEND_MANIPULATIONS_MANIPULATIONOPTIONS_H

//
//  xml_express.h
//  Mapport
//
//  Created by MoK on 15-2-5.
//  Copyright (c) 2015骞� MoK. All rights reserved.
//

#ifndef __Mapport__xml_express__
#define __Mapport__xml_express__

#include <stdio.h>
//#include "xml_app.h"
#include "Header.h"


struct	timespec xml_config_time;
#endif /* defined(__Mapport__xml_express__) */

int writedata_to_xml(char *xml_name, map_xml_def *pdev_stru, int data_size);
int readata_from_xml(char *xml_name, map_xml_def *pdev_stru, int data_size);
int check_file_time(void);
int compare_file_time(void);
int adddata_to_xml(char *xml_name, map_xml_def *pdev_stru, int ex_len);
int deldata_from_xml(char *xml_name, map_xml_def *pdev_stru, int ex_len);

#ifndef XAODJETRECLUSTERING_IJETRECLUSTERINGTOOL_H
#define XAODJETRECLUSTERING_IJETRECLUSTERINGTOOL_H 1

#include "AsgTools/IAsgTool.h"

class IJetReclusteringTool : public virtual asg::IAsgTool {
   public:
      ASG_TOOL_INTERFACE( IJetReclusteringTool ) //declares the interface to athena

      // execute reclustering
      virtual void execute() const = 0;
};

#endif //> !XAODJETRECLUSTERING_IJETRECLUSTERINGTOOL_H

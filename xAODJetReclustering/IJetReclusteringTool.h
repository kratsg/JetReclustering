#ifndef XAODJETRECLUSTERING_IJETRECLUSTERINGTOOL_H
#define XAODJETRECLUSTERING_IJETRECLUSTERINGTOOL_H 1

#include "AsgTools/IAsgTool.h"

#include "xAODBase/IParticle.h"

class IJetReclusteringTool : public virtual asg::IAsgTool {
   public:
      ASG_TOOL_INTERFACE( IJetReclusteringTool ) //declares the interface to athena

      //below is an example method, delete as necessary
      //this is where you specify default values for additional arguments
      virtual bool isSelected(const xAOD::IParticle& particle, bool someAdditionalArgument = false) const = 0;

      //declare enums for properties here too, so that they are accessible through just the interface header
      //example of an enum you might end up using for your properties .. please put enums in a class! (not just a namespace)
      enum MyEnumProperty { Val1=1 , Val2=2 };

};

#endif //> !XAODJETRECLUSTERING_IJETRECLUSTERINGTOOL_H

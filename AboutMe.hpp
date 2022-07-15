//
//  Created by rick gessner on 3/23/22.
//

#ifndef AboutMe_hpp
#define AboutMe_hpp

#include <stdio.h>
#include <string>

namespace ECE141 {
  
  class AboutMe {
  public:
    AboutMe() : name("Yen-Ju Tseng"), pid("A59005785") {} 
    
    const std::string& getPID() const {return pid;}
    const std::string& getName() const {return name;}

  protected:
    std::string name;
    std::string pid;
  };

}

#endif /* about_me */

#include <stdlib.h>
#include <Wire.h>

class i2c_Mux
{
  private:
    int mux_addr; //address of the mux
    //int active_channel; //Channel being used
  
  public:
    i2c_Mux(int);
    void setActiveChannel(int);   
};

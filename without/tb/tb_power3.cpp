#include <stdlib.h>
#include <iostream>
#include <cstdlib>

#include <verilated.h>         // Common verilator routines.
#include <verilated_vcd_c.h>   // Write waverforms to a VCD file.
#include "Vpower3.h" // Verilated DUT.

#define MAX_SIM_TIME 300
#define RESET_NEG_EDGE 2
#define VERIF_START_TIME 7

vluint64_t sim_time    = 0;
vluint64_t posedge_cnt = 0;

// Assert srst only on the first clock edge.
void dut_reset (Vpower3 *dut)
{
  dut->i_srst  = 0;
  dut->i_clkEn = 1;

  if (sim_time <= 2)
  {
	  dut->i_srst  = 1;
	  dut->i_clkEn = 0;
    dut->i_x     = 0;
  }
}

// Randomise i_x between 0 to 6 every 3 clk cycles.
void randomise_input(Vpower3 *dut)
{
  if ((posedge_cnt%3 == 0) && (sim_time > RESET_NEG_EDGE))
  {
    dut->i_x = rand()%7;
  }
}

// Confirm o_xPower and hence the x^3 module works as expected.
void verify_output(Vpower3 *dut)
{
  if ((posedge_cnt%3 == 0) && (sim_time > VERIF_START_TIME))
  {
   int expectedResult =  (dut->i_x) * (dut->i_x) * (dut->i_x);
   if (dut->o_xPower != expectedResult)
   {
    std::cout << "ERROR: o_xPower is incorrect, "
    << "expected: " << expectedResult
    << " recv: " << (int)dut->o_xPower
    << " simtime: " << sim_time << std::endl;
   }
  }
}

int main(int argc, char** argv, char** env)
{
  srand (time(NULL));
  Verilated::commandArgs(argc, argv);
  Vpower3 *dut = new Vpower3; // Instantiate DUT.

  // {{{ Set-up waveform dumping.

    Verilated::traceEverOn(true);
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

  // }}} Set-up waveform dumping.

  while (sim_time < MAX_SIM_TIME)
  {
    dut->i_clk ^= 1;

    dut->eval(); // Evaluate all the signals in the DUT on each clock edge.

    if (dut->i_clk == 1)
    {
      posedge_cnt++;

      dut_reset(dut);
      verify_output(dut);
      randomise_input(dut);
    }

    m_trace->dump(sim_time); // Write all the traced signal values into the waveform dump file.
    sim_time++;
  }

  m_trace->close();
  delete dut;
  exit(EXIT_SUCCESS);
}

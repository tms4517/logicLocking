// This module calculate the cube of i_x. Note: i_clkEn and i_srst ports are
// required for custom hardware blocks to be interfaced to a NiosII soft-core
// processor.

`default_nettype none

module power3
  ( input  var logic i_clk
  , input  var logic i_clkEn
  , input  var logic i_srst

  , input  var logic [31:0] i_x

  , output var logic [31:0] o_xPower
  );

  logic [31:0] xPower1_q, x2_q, xPower2_q, xPower3_q;

  assign o_xPower = xPower3_q;

  // Pipeline stage 1. X
  always_ff @(posedge i_clk)
    if (i_srst)
      xPower1_q <= '0;
    else if (i_clkEn)
      xPower1_q <= i_x;
    else
      xPower1_q <= xPower1_q;

  // X delayed by 2 clock cycles.
  always_ff @(posedge i_clk)
    if (i_srst)
      x2_q <= '0;
    else if (i_clkEn)
      x2_q <= xPower1_q;
    else
      x2_q <= x2_q;

  // Pipeline stage 2. X^2
  always_ff @(posedge i_clk)
    if (i_srst)
      xPower2_q <= '0;
    else if (i_clkEn)
      xPower2_q <= xPower1_q*xPower1_q;
    else
      xPower2_q <= xPower2_q;

  // Pipeline stage 3. X^3
  always_ff @(posedge i_clk)
    if (i_srst)
      xPower3_q <= '0;
    else if (i_clkEn)
      xPower3_q <= xPower2_q*x2_q;
    else
      xPower3_q <= xPower3_q;

endmodule

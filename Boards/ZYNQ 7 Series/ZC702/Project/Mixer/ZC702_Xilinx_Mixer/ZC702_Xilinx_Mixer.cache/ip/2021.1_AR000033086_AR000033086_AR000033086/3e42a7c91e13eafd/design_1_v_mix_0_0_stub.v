// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2021.1_AR000033086_AR000033086_AR000033086 (win64) Build 3247384 Thu Jun 10 19:36:33 MDT 2021
// Date        : Mon Oct 30 12:15:11 2023
// Host        : IBM-481 running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ design_1_v_mix_0_0_stub.v
// Design      : design_1_v_mix_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z020clg484-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "design_1_v_mix_0_0_v_mix,Vivado 2021.1_AR000033086_AR000033086_AR000033086" *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix(s_axi_CTRL_AWADDR, s_axi_CTRL_AWVALID, 
  s_axi_CTRL_AWREADY, s_axi_CTRL_WDATA, s_axi_CTRL_WSTRB, s_axi_CTRL_WVALID, 
  s_axi_CTRL_WREADY, s_axi_CTRL_BRESP, s_axi_CTRL_BVALID, s_axi_CTRL_BREADY, 
  s_axi_CTRL_ARADDR, s_axi_CTRL_ARVALID, s_axi_CTRL_ARREADY, s_axi_CTRL_RDATA, 
  s_axi_CTRL_RRESP, s_axi_CTRL_RVALID, s_axi_CTRL_RREADY, ap_clk, ap_rst_n, interrupt, 
  m_axi_mm_video1_AWID, m_axi_mm_video1_AWADDR, m_axi_mm_video1_AWLEN, 
  m_axi_mm_video1_AWSIZE, m_axi_mm_video1_AWBURST, m_axi_mm_video1_AWLOCK, 
  m_axi_mm_video1_AWREGION, m_axi_mm_video1_AWCACHE, m_axi_mm_video1_AWPROT, 
  m_axi_mm_video1_AWQOS, m_axi_mm_video1_AWVALID, m_axi_mm_video1_AWREADY, 
  m_axi_mm_video1_WID, m_axi_mm_video1_WDATA, m_axi_mm_video1_WSTRB, 
  m_axi_mm_video1_WLAST, m_axi_mm_video1_WVALID, m_axi_mm_video1_WREADY, 
  m_axi_mm_video1_BID, m_axi_mm_video1_BRESP, m_axi_mm_video1_BVALID, 
  m_axi_mm_video1_BREADY, m_axi_mm_video1_ARID, m_axi_mm_video1_ARADDR, 
  m_axi_mm_video1_ARLEN, m_axi_mm_video1_ARSIZE, m_axi_mm_video1_ARBURST, 
  m_axi_mm_video1_ARLOCK, m_axi_mm_video1_ARREGION, m_axi_mm_video1_ARCACHE, 
  m_axi_mm_video1_ARPROT, m_axi_mm_video1_ARQOS, m_axi_mm_video1_ARVALID, 
  m_axi_mm_video1_ARREADY, m_axi_mm_video1_RID, m_axi_mm_video1_RDATA, 
  m_axi_mm_video1_RRESP, m_axi_mm_video1_RLAST, m_axi_mm_video1_RVALID, 
  m_axi_mm_video1_RREADY, m_axi_mm_video2_AWID, m_axi_mm_video2_AWADDR, 
  m_axi_mm_video2_AWLEN, m_axi_mm_video2_AWSIZE, m_axi_mm_video2_AWBURST, 
  m_axi_mm_video2_AWLOCK, m_axi_mm_video2_AWREGION, m_axi_mm_video2_AWCACHE, 
  m_axi_mm_video2_AWPROT, m_axi_mm_video2_AWQOS, m_axi_mm_video2_AWVALID, 
  m_axi_mm_video2_AWREADY, m_axi_mm_video2_WID, m_axi_mm_video2_WDATA, 
  m_axi_mm_video2_WSTRB, m_axi_mm_video2_WLAST, m_axi_mm_video2_WVALID, 
  m_axi_mm_video2_WREADY, m_axi_mm_video2_BID, m_axi_mm_video2_BRESP, 
  m_axi_mm_video2_BVALID, m_axi_mm_video2_BREADY, m_axi_mm_video2_ARID, 
  m_axi_mm_video2_ARADDR, m_axi_mm_video2_ARLEN, m_axi_mm_video2_ARSIZE, 
  m_axi_mm_video2_ARBURST, m_axi_mm_video2_ARLOCK, m_axi_mm_video2_ARREGION, 
  m_axi_mm_video2_ARCACHE, m_axi_mm_video2_ARPROT, m_axi_mm_video2_ARQOS, 
  m_axi_mm_video2_ARVALID, m_axi_mm_video2_ARREADY, m_axi_mm_video2_RID, 
  m_axi_mm_video2_RDATA, m_axi_mm_video2_RRESP, m_axi_mm_video2_RLAST, 
  m_axi_mm_video2_RVALID, m_axi_mm_video2_RREADY, m_axi_mm_video3_AWID, 
  m_axi_mm_video3_AWADDR, m_axi_mm_video3_AWLEN, m_axi_mm_video3_AWSIZE, 
  m_axi_mm_video3_AWBURST, m_axi_mm_video3_AWLOCK, m_axi_mm_video3_AWREGION, 
  m_axi_mm_video3_AWCACHE, m_axi_mm_video3_AWPROT, m_axi_mm_video3_AWQOS, 
  m_axi_mm_video3_AWVALID, m_axi_mm_video3_AWREADY, m_axi_mm_video3_WID, 
  m_axi_mm_video3_WDATA, m_axi_mm_video3_WSTRB, m_axi_mm_video3_WLAST, 
  m_axi_mm_video3_WVALID, m_axi_mm_video3_WREADY, m_axi_mm_video3_BID, 
  m_axi_mm_video3_BRESP, m_axi_mm_video3_BVALID, m_axi_mm_video3_BREADY, 
  m_axi_mm_video3_ARID, m_axi_mm_video3_ARADDR, m_axi_mm_video3_ARLEN, 
  m_axi_mm_video3_ARSIZE, m_axi_mm_video3_ARBURST, m_axi_mm_video3_ARLOCK, 
  m_axi_mm_video3_ARREGION, m_axi_mm_video3_ARCACHE, m_axi_mm_video3_ARPROT, 
  m_axi_mm_video3_ARQOS, m_axi_mm_video3_ARVALID, m_axi_mm_video3_ARREADY, 
  m_axi_mm_video3_RID, m_axi_mm_video3_RDATA, m_axi_mm_video3_RRESP, 
  m_axi_mm_video3_RLAST, m_axi_mm_video3_RVALID, m_axi_mm_video3_RREADY, 
  s_axis_video_TVALID, s_axis_video_TREADY, s_axis_video_TDATA, s_axis_video_TKEEP, 
  s_axis_video_TSTRB, s_axis_video_TUSER, s_axis_video_TLAST, s_axis_video_TID, 
  s_axis_video_TDEST, m_axis_video_TVALID, m_axis_video_TREADY, m_axis_video_TDATA, 
  m_axis_video_TKEEP, m_axis_video_TSTRB, m_axis_video_TUSER, m_axis_video_TLAST, 
  m_axis_video_TID, m_axis_video_TDEST)
/* synthesis syn_black_box black_box_pad_pin="s_axi_CTRL_AWADDR[12:0],s_axi_CTRL_AWVALID,s_axi_CTRL_AWREADY,s_axi_CTRL_WDATA[31:0],s_axi_CTRL_WSTRB[3:0],s_axi_CTRL_WVALID,s_axi_CTRL_WREADY,s_axi_CTRL_BRESP[1:0],s_axi_CTRL_BVALID,s_axi_CTRL_BREADY,s_axi_CTRL_ARADDR[12:0],s_axi_CTRL_ARVALID,s_axi_CTRL_ARREADY,s_axi_CTRL_RDATA[31:0],s_axi_CTRL_RRESP[1:0],s_axi_CTRL_RVALID,s_axi_CTRL_RREADY,ap_clk,ap_rst_n,interrupt,m_axi_mm_video1_AWID[0:0],m_axi_mm_video1_AWADDR[31:0],m_axi_mm_video1_AWLEN[7:0],m_axi_mm_video1_AWSIZE[2:0],m_axi_mm_video1_AWBURST[1:0],m_axi_mm_video1_AWLOCK[1:0],m_axi_mm_video1_AWREGION[3:0],m_axi_mm_video1_AWCACHE[3:0],m_axi_mm_video1_AWPROT[2:0],m_axi_mm_video1_AWQOS[3:0],m_axi_mm_video1_AWVALID,m_axi_mm_video1_AWREADY,m_axi_mm_video1_WID[0:0],m_axi_mm_video1_WDATA[63:0],m_axi_mm_video1_WSTRB[7:0],m_axi_mm_video1_WLAST,m_axi_mm_video1_WVALID,m_axi_mm_video1_WREADY,m_axi_mm_video1_BID[0:0],m_axi_mm_video1_BRESP[1:0],m_axi_mm_video1_BVALID,m_axi_mm_video1_BREADY,m_axi_mm_video1_ARID[0:0],m_axi_mm_video1_ARADDR[31:0],m_axi_mm_video1_ARLEN[7:0],m_axi_mm_video1_ARSIZE[2:0],m_axi_mm_video1_ARBURST[1:0],m_axi_mm_video1_ARLOCK[1:0],m_axi_mm_video1_ARREGION[3:0],m_axi_mm_video1_ARCACHE[3:0],m_axi_mm_video1_ARPROT[2:0],m_axi_mm_video1_ARQOS[3:0],m_axi_mm_video1_ARVALID,m_axi_mm_video1_ARREADY,m_axi_mm_video1_RID[0:0],m_axi_mm_video1_RDATA[63:0],m_axi_mm_video1_RRESP[1:0],m_axi_mm_video1_RLAST,m_axi_mm_video1_RVALID,m_axi_mm_video1_RREADY,m_axi_mm_video2_AWID[0:0],m_axi_mm_video2_AWADDR[31:0],m_axi_mm_video2_AWLEN[7:0],m_axi_mm_video2_AWSIZE[2:0],m_axi_mm_video2_AWBURST[1:0],m_axi_mm_video2_AWLOCK[1:0],m_axi_mm_video2_AWREGION[3:0],m_axi_mm_video2_AWCACHE[3:0],m_axi_mm_video2_AWPROT[2:0],m_axi_mm_video2_AWQOS[3:0],m_axi_mm_video2_AWVALID,m_axi_mm_video2_AWREADY,m_axi_mm_video2_WID[0:0],m_axi_mm_video2_WDATA[63:0],m_axi_mm_video2_WSTRB[7:0],m_axi_mm_video2_WLAST,m_axi_mm_video2_WVALID,m_axi_mm_video2_WREADY,m_axi_mm_video2_BID[0:0],m_axi_mm_video2_BRESP[1:0],m_axi_mm_video2_BVALID,m_axi_mm_video2_BREADY,m_axi_mm_video2_ARID[0:0],m_axi_mm_video2_ARADDR[31:0],m_axi_mm_video2_ARLEN[7:0],m_axi_mm_video2_ARSIZE[2:0],m_axi_mm_video2_ARBURST[1:0],m_axi_mm_video2_ARLOCK[1:0],m_axi_mm_video2_ARREGION[3:0],m_axi_mm_video2_ARCACHE[3:0],m_axi_mm_video2_ARPROT[2:0],m_axi_mm_video2_ARQOS[3:0],m_axi_mm_video2_ARVALID,m_axi_mm_video2_ARREADY,m_axi_mm_video2_RID[0:0],m_axi_mm_video2_RDATA[63:0],m_axi_mm_video2_RRESP[1:0],m_axi_mm_video2_RLAST,m_axi_mm_video2_RVALID,m_axi_mm_video2_RREADY,m_axi_mm_video3_AWID[0:0],m_axi_mm_video3_AWADDR[31:0],m_axi_mm_video3_AWLEN[7:0],m_axi_mm_video3_AWSIZE[2:0],m_axi_mm_video3_AWBURST[1:0],m_axi_mm_video3_AWLOCK[1:0],m_axi_mm_video3_AWREGION[3:0],m_axi_mm_video3_AWCACHE[3:0],m_axi_mm_video3_AWPROT[2:0],m_axi_mm_video3_AWQOS[3:0],m_axi_mm_video3_AWVALID,m_axi_mm_video3_AWREADY,m_axi_mm_video3_WID[0:0],m_axi_mm_video3_WDATA[63:0],m_axi_mm_video3_WSTRB[7:0],m_axi_mm_video3_WLAST,m_axi_mm_video3_WVALID,m_axi_mm_video3_WREADY,m_axi_mm_video3_BID[0:0],m_axi_mm_video3_BRESP[1:0],m_axi_mm_video3_BVALID,m_axi_mm_video3_BREADY,m_axi_mm_video3_ARID[0:0],m_axi_mm_video3_ARADDR[31:0],m_axi_mm_video3_ARLEN[7:0],m_axi_mm_video3_ARSIZE[2:0],m_axi_mm_video3_ARBURST[1:0],m_axi_mm_video3_ARLOCK[1:0],m_axi_mm_video3_ARREGION[3:0],m_axi_mm_video3_ARCACHE[3:0],m_axi_mm_video3_ARPROT[2:0],m_axi_mm_video3_ARQOS[3:0],m_axi_mm_video3_ARVALID,m_axi_mm_video3_ARREADY,m_axi_mm_video3_RID[0:0],m_axi_mm_video3_RDATA[63:0],m_axi_mm_video3_RRESP[1:0],m_axi_mm_video3_RLAST,m_axi_mm_video3_RVALID,m_axi_mm_video3_RREADY,s_axis_video_TVALID,s_axis_video_TREADY,s_axis_video_TDATA[23:0],s_axis_video_TKEEP[2:0],s_axis_video_TSTRB[2:0],s_axis_video_TUSER[0:0],s_axis_video_TLAST[0:0],s_axis_video_TID[0:0],s_axis_video_TDEST[0:0],m_axis_video_TVALID,m_axis_video_TREADY,m_axis_video_TDATA[23:0],m_axis_video_TKEEP[2:0],m_axis_video_TSTRB[2:0],m_axis_video_TUSER[0:0],m_axis_video_TLAST[0:0],m_axis_video_TID[0:0],m_axis_video_TDEST[0:0]" */;
  input [12:0]s_axi_CTRL_AWADDR;
  input s_axi_CTRL_AWVALID;
  output s_axi_CTRL_AWREADY;
  input [31:0]s_axi_CTRL_WDATA;
  input [3:0]s_axi_CTRL_WSTRB;
  input s_axi_CTRL_WVALID;
  output s_axi_CTRL_WREADY;
  output [1:0]s_axi_CTRL_BRESP;
  output s_axi_CTRL_BVALID;
  input s_axi_CTRL_BREADY;
  input [12:0]s_axi_CTRL_ARADDR;
  input s_axi_CTRL_ARVALID;
  output s_axi_CTRL_ARREADY;
  output [31:0]s_axi_CTRL_RDATA;
  output [1:0]s_axi_CTRL_RRESP;
  output s_axi_CTRL_RVALID;
  input s_axi_CTRL_RREADY;
  input ap_clk;
  input ap_rst_n;
  output interrupt;
  output [0:0]m_axi_mm_video1_AWID;
  output [31:0]m_axi_mm_video1_AWADDR;
  output [7:0]m_axi_mm_video1_AWLEN;
  output [2:0]m_axi_mm_video1_AWSIZE;
  output [1:0]m_axi_mm_video1_AWBURST;
  output [1:0]m_axi_mm_video1_AWLOCK;
  output [3:0]m_axi_mm_video1_AWREGION;
  output [3:0]m_axi_mm_video1_AWCACHE;
  output [2:0]m_axi_mm_video1_AWPROT;
  output [3:0]m_axi_mm_video1_AWQOS;
  output m_axi_mm_video1_AWVALID;
  input m_axi_mm_video1_AWREADY;
  output [0:0]m_axi_mm_video1_WID;
  output [63:0]m_axi_mm_video1_WDATA;
  output [7:0]m_axi_mm_video1_WSTRB;
  output m_axi_mm_video1_WLAST;
  output m_axi_mm_video1_WVALID;
  input m_axi_mm_video1_WREADY;
  input [0:0]m_axi_mm_video1_BID;
  input [1:0]m_axi_mm_video1_BRESP;
  input m_axi_mm_video1_BVALID;
  output m_axi_mm_video1_BREADY;
  output [0:0]m_axi_mm_video1_ARID;
  output [31:0]m_axi_mm_video1_ARADDR;
  output [7:0]m_axi_mm_video1_ARLEN;
  output [2:0]m_axi_mm_video1_ARSIZE;
  output [1:0]m_axi_mm_video1_ARBURST;
  output [1:0]m_axi_mm_video1_ARLOCK;
  output [3:0]m_axi_mm_video1_ARREGION;
  output [3:0]m_axi_mm_video1_ARCACHE;
  output [2:0]m_axi_mm_video1_ARPROT;
  output [3:0]m_axi_mm_video1_ARQOS;
  output m_axi_mm_video1_ARVALID;
  input m_axi_mm_video1_ARREADY;
  input [0:0]m_axi_mm_video1_RID;
  input [63:0]m_axi_mm_video1_RDATA;
  input [1:0]m_axi_mm_video1_RRESP;
  input m_axi_mm_video1_RLAST;
  input m_axi_mm_video1_RVALID;
  output m_axi_mm_video1_RREADY;
  output [0:0]m_axi_mm_video2_AWID;
  output [31:0]m_axi_mm_video2_AWADDR;
  output [7:0]m_axi_mm_video2_AWLEN;
  output [2:0]m_axi_mm_video2_AWSIZE;
  output [1:0]m_axi_mm_video2_AWBURST;
  output [1:0]m_axi_mm_video2_AWLOCK;
  output [3:0]m_axi_mm_video2_AWREGION;
  output [3:0]m_axi_mm_video2_AWCACHE;
  output [2:0]m_axi_mm_video2_AWPROT;
  output [3:0]m_axi_mm_video2_AWQOS;
  output m_axi_mm_video2_AWVALID;
  input m_axi_mm_video2_AWREADY;
  output [0:0]m_axi_mm_video2_WID;
  output [63:0]m_axi_mm_video2_WDATA;
  output [7:0]m_axi_mm_video2_WSTRB;
  output m_axi_mm_video2_WLAST;
  output m_axi_mm_video2_WVALID;
  input m_axi_mm_video2_WREADY;
  input [0:0]m_axi_mm_video2_BID;
  input [1:0]m_axi_mm_video2_BRESP;
  input m_axi_mm_video2_BVALID;
  output m_axi_mm_video2_BREADY;
  output [0:0]m_axi_mm_video2_ARID;
  output [31:0]m_axi_mm_video2_ARADDR;
  output [7:0]m_axi_mm_video2_ARLEN;
  output [2:0]m_axi_mm_video2_ARSIZE;
  output [1:0]m_axi_mm_video2_ARBURST;
  output [1:0]m_axi_mm_video2_ARLOCK;
  output [3:0]m_axi_mm_video2_ARREGION;
  output [3:0]m_axi_mm_video2_ARCACHE;
  output [2:0]m_axi_mm_video2_ARPROT;
  output [3:0]m_axi_mm_video2_ARQOS;
  output m_axi_mm_video2_ARVALID;
  input m_axi_mm_video2_ARREADY;
  input [0:0]m_axi_mm_video2_RID;
  input [63:0]m_axi_mm_video2_RDATA;
  input [1:0]m_axi_mm_video2_RRESP;
  input m_axi_mm_video2_RLAST;
  input m_axi_mm_video2_RVALID;
  output m_axi_mm_video2_RREADY;
  output [0:0]m_axi_mm_video3_AWID;
  output [31:0]m_axi_mm_video3_AWADDR;
  output [7:0]m_axi_mm_video3_AWLEN;
  output [2:0]m_axi_mm_video3_AWSIZE;
  output [1:0]m_axi_mm_video3_AWBURST;
  output [1:0]m_axi_mm_video3_AWLOCK;
  output [3:0]m_axi_mm_video3_AWREGION;
  output [3:0]m_axi_mm_video3_AWCACHE;
  output [2:0]m_axi_mm_video3_AWPROT;
  output [3:0]m_axi_mm_video3_AWQOS;
  output m_axi_mm_video3_AWVALID;
  input m_axi_mm_video3_AWREADY;
  output [0:0]m_axi_mm_video3_WID;
  output [63:0]m_axi_mm_video3_WDATA;
  output [7:0]m_axi_mm_video3_WSTRB;
  output m_axi_mm_video3_WLAST;
  output m_axi_mm_video3_WVALID;
  input m_axi_mm_video3_WREADY;
  input [0:0]m_axi_mm_video3_BID;
  input [1:0]m_axi_mm_video3_BRESP;
  input m_axi_mm_video3_BVALID;
  output m_axi_mm_video3_BREADY;
  output [0:0]m_axi_mm_video3_ARID;
  output [31:0]m_axi_mm_video3_ARADDR;
  output [7:0]m_axi_mm_video3_ARLEN;
  output [2:0]m_axi_mm_video3_ARSIZE;
  output [1:0]m_axi_mm_video3_ARBURST;
  output [1:0]m_axi_mm_video3_ARLOCK;
  output [3:0]m_axi_mm_video3_ARREGION;
  output [3:0]m_axi_mm_video3_ARCACHE;
  output [2:0]m_axi_mm_video3_ARPROT;
  output [3:0]m_axi_mm_video3_ARQOS;
  output m_axi_mm_video3_ARVALID;
  input m_axi_mm_video3_ARREADY;
  input [0:0]m_axi_mm_video3_RID;
  input [63:0]m_axi_mm_video3_RDATA;
  input [1:0]m_axi_mm_video3_RRESP;
  input m_axi_mm_video3_RLAST;
  input m_axi_mm_video3_RVALID;
  output m_axi_mm_video3_RREADY;
  input s_axis_video_TVALID;
  output s_axis_video_TREADY;
  input [23:0]s_axis_video_TDATA;
  input [2:0]s_axis_video_TKEEP;
  input [2:0]s_axis_video_TSTRB;
  input [0:0]s_axis_video_TUSER;
  input [0:0]s_axis_video_TLAST;
  input [0:0]s_axis_video_TID;
  input [0:0]s_axis_video_TDEST;
  output m_axis_video_TVALID;
  input m_axis_video_TREADY;
  output [23:0]m_axis_video_TDATA;
  output [2:0]m_axis_video_TKEEP;
  output [2:0]m_axis_video_TSTRB;
  output [0:0]m_axis_video_TUSER;
  output [0:0]m_axis_video_TLAST;
  output [0:0]m_axis_video_TID;
  output [0:0]m_axis_video_TDEST;
endmodule

-- Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2021.1_AR000033086_AR000033086_AR000033086 (win64) Build 3247384 Thu Jun 10 19:36:33 MDT 2021
-- Date        : Mon Oct 30 12:15:11 2023
-- Host        : IBM-481 running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub -rename_top design_1_v_mix_0_1 -prefix
--               design_1_v_mix_0_1_ design_1_v_mix_0_0_stub.vhdl
-- Design      : design_1_v_mix_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7z020clg484-1
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity design_1_v_mix_0_1 is
  Port ( 
    s_axi_CTRL_AWADDR : in STD_LOGIC_VECTOR ( 12 downto 0 );
    s_axi_CTRL_AWVALID : in STD_LOGIC;
    s_axi_CTRL_AWREADY : out STD_LOGIC;
    s_axi_CTRL_WDATA : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_CTRL_WSTRB : in STD_LOGIC_VECTOR ( 3 downto 0 );
    s_axi_CTRL_WVALID : in STD_LOGIC;
    s_axi_CTRL_WREADY : out STD_LOGIC;
    s_axi_CTRL_BRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_CTRL_BVALID : out STD_LOGIC;
    s_axi_CTRL_BREADY : in STD_LOGIC;
    s_axi_CTRL_ARADDR : in STD_LOGIC_VECTOR ( 12 downto 0 );
    s_axi_CTRL_ARVALID : in STD_LOGIC;
    s_axi_CTRL_ARREADY : out STD_LOGIC;
    s_axi_CTRL_RDATA : out STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_CTRL_RRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_CTRL_RVALID : out STD_LOGIC;
    s_axi_CTRL_RREADY : in STD_LOGIC;
    ap_clk : in STD_LOGIC;
    ap_rst_n : in STD_LOGIC;
    interrupt : out STD_LOGIC;
    m_axi_mm_video1_AWID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video1_AWADDR : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_mm_video1_AWLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video1_AWSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video1_AWBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video1_AWLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video1_AWREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video1_AWCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video1_AWPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video1_AWQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video1_AWVALID : out STD_LOGIC;
    m_axi_mm_video1_AWREADY : in STD_LOGIC;
    m_axi_mm_video1_WID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video1_WDATA : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_mm_video1_WSTRB : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video1_WLAST : out STD_LOGIC;
    m_axi_mm_video1_WVALID : out STD_LOGIC;
    m_axi_mm_video1_WREADY : in STD_LOGIC;
    m_axi_mm_video1_BID : in STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video1_BRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video1_BVALID : in STD_LOGIC;
    m_axi_mm_video1_BREADY : out STD_LOGIC;
    m_axi_mm_video1_ARID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video1_ARADDR : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_mm_video1_ARLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video1_ARSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video1_ARBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video1_ARLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video1_ARREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video1_ARCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video1_ARPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video1_ARQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video1_ARVALID : out STD_LOGIC;
    m_axi_mm_video1_ARREADY : in STD_LOGIC;
    m_axi_mm_video1_RID : in STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video1_RDATA : in STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_mm_video1_RRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video1_RLAST : in STD_LOGIC;
    m_axi_mm_video1_RVALID : in STD_LOGIC;
    m_axi_mm_video1_RREADY : out STD_LOGIC;
    m_axi_mm_video2_AWID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video2_AWADDR : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_mm_video2_AWLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video2_AWSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video2_AWBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video2_AWLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video2_AWREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video2_AWCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video2_AWPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video2_AWQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video2_AWVALID : out STD_LOGIC;
    m_axi_mm_video2_AWREADY : in STD_LOGIC;
    m_axi_mm_video2_WID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video2_WDATA : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_mm_video2_WSTRB : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video2_WLAST : out STD_LOGIC;
    m_axi_mm_video2_WVALID : out STD_LOGIC;
    m_axi_mm_video2_WREADY : in STD_LOGIC;
    m_axi_mm_video2_BID : in STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video2_BRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video2_BVALID : in STD_LOGIC;
    m_axi_mm_video2_BREADY : out STD_LOGIC;
    m_axi_mm_video2_ARID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video2_ARADDR : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_mm_video2_ARLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video2_ARSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video2_ARBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video2_ARLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video2_ARREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video2_ARCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video2_ARPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video2_ARQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video2_ARVALID : out STD_LOGIC;
    m_axi_mm_video2_ARREADY : in STD_LOGIC;
    m_axi_mm_video2_RID : in STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video2_RDATA : in STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_mm_video2_RRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video2_RLAST : in STD_LOGIC;
    m_axi_mm_video2_RVALID : in STD_LOGIC;
    m_axi_mm_video2_RREADY : out STD_LOGIC;
    m_axi_mm_video3_AWID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video3_AWADDR : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_mm_video3_AWLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video3_AWSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video3_AWBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video3_AWLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video3_AWREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video3_AWCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video3_AWPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video3_AWQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video3_AWVALID : out STD_LOGIC;
    m_axi_mm_video3_AWREADY : in STD_LOGIC;
    m_axi_mm_video3_WID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video3_WDATA : out STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_mm_video3_WSTRB : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video3_WLAST : out STD_LOGIC;
    m_axi_mm_video3_WVALID : out STD_LOGIC;
    m_axi_mm_video3_WREADY : in STD_LOGIC;
    m_axi_mm_video3_BID : in STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video3_BRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video3_BVALID : in STD_LOGIC;
    m_axi_mm_video3_BREADY : out STD_LOGIC;
    m_axi_mm_video3_ARID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video3_ARADDR : out STD_LOGIC_VECTOR ( 31 downto 0 );
    m_axi_mm_video3_ARLEN : out STD_LOGIC_VECTOR ( 7 downto 0 );
    m_axi_mm_video3_ARSIZE : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video3_ARBURST : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video3_ARLOCK : out STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video3_ARREGION : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video3_ARCACHE : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video3_ARPROT : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axi_mm_video3_ARQOS : out STD_LOGIC_VECTOR ( 3 downto 0 );
    m_axi_mm_video3_ARVALID : out STD_LOGIC;
    m_axi_mm_video3_ARREADY : in STD_LOGIC;
    m_axi_mm_video3_RID : in STD_LOGIC_VECTOR ( 0 to 0 );
    m_axi_mm_video3_RDATA : in STD_LOGIC_VECTOR ( 63 downto 0 );
    m_axi_mm_video3_RRESP : in STD_LOGIC_VECTOR ( 1 downto 0 );
    m_axi_mm_video3_RLAST : in STD_LOGIC;
    m_axi_mm_video3_RVALID : in STD_LOGIC;
    m_axi_mm_video3_RREADY : out STD_LOGIC;
    s_axis_video_TVALID : in STD_LOGIC;
    s_axis_video_TREADY : out STD_LOGIC;
    s_axis_video_TDATA : in STD_LOGIC_VECTOR ( 23 downto 0 );
    s_axis_video_TKEEP : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axis_video_TSTRB : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axis_video_TUSER : in STD_LOGIC_VECTOR ( 0 to 0 );
    s_axis_video_TLAST : in STD_LOGIC_VECTOR ( 0 to 0 );
    s_axis_video_TID : in STD_LOGIC_VECTOR ( 0 to 0 );
    s_axis_video_TDEST : in STD_LOGIC_VECTOR ( 0 to 0 );
    m_axis_video_TVALID : out STD_LOGIC;
    m_axis_video_TREADY : in STD_LOGIC;
    m_axis_video_TDATA : out STD_LOGIC_VECTOR ( 23 downto 0 );
    m_axis_video_TKEEP : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axis_video_TSTRB : out STD_LOGIC_VECTOR ( 2 downto 0 );
    m_axis_video_TUSER : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axis_video_TLAST : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axis_video_TID : out STD_LOGIC_VECTOR ( 0 to 0 );
    m_axis_video_TDEST : out STD_LOGIC_VECTOR ( 0 to 0 )
  );

end design_1_v_mix_0_1;

architecture stub of design_1_v_mix_0_1 is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "s_axi_CTRL_AWADDR[12:0],s_axi_CTRL_AWVALID,s_axi_CTRL_AWREADY,s_axi_CTRL_WDATA[31:0],s_axi_CTRL_WSTRB[3:0],s_axi_CTRL_WVALID,s_axi_CTRL_WREADY,s_axi_CTRL_BRESP[1:0],s_axi_CTRL_BVALID,s_axi_CTRL_BREADY,s_axi_CTRL_ARADDR[12:0],s_axi_CTRL_ARVALID,s_axi_CTRL_ARREADY,s_axi_CTRL_RDATA[31:0],s_axi_CTRL_RRESP[1:0],s_axi_CTRL_RVALID,s_axi_CTRL_RREADY,ap_clk,ap_rst_n,interrupt,m_axi_mm_video1_AWID[0:0],m_axi_mm_video1_AWADDR[31:0],m_axi_mm_video1_AWLEN[7:0],m_axi_mm_video1_AWSIZE[2:0],m_axi_mm_video1_AWBURST[1:0],m_axi_mm_video1_AWLOCK[1:0],m_axi_mm_video1_AWREGION[3:0],m_axi_mm_video1_AWCACHE[3:0],m_axi_mm_video1_AWPROT[2:0],m_axi_mm_video1_AWQOS[3:0],m_axi_mm_video1_AWVALID,m_axi_mm_video1_AWREADY,m_axi_mm_video1_WID[0:0],m_axi_mm_video1_WDATA[63:0],m_axi_mm_video1_WSTRB[7:0],m_axi_mm_video1_WLAST,m_axi_mm_video1_WVALID,m_axi_mm_video1_WREADY,m_axi_mm_video1_BID[0:0],m_axi_mm_video1_BRESP[1:0],m_axi_mm_video1_BVALID,m_axi_mm_video1_BREADY,m_axi_mm_video1_ARID[0:0],m_axi_mm_video1_ARADDR[31:0],m_axi_mm_video1_ARLEN[7:0],m_axi_mm_video1_ARSIZE[2:0],m_axi_mm_video1_ARBURST[1:0],m_axi_mm_video1_ARLOCK[1:0],m_axi_mm_video1_ARREGION[3:0],m_axi_mm_video1_ARCACHE[3:0],m_axi_mm_video1_ARPROT[2:0],m_axi_mm_video1_ARQOS[3:0],m_axi_mm_video1_ARVALID,m_axi_mm_video1_ARREADY,m_axi_mm_video1_RID[0:0],m_axi_mm_video1_RDATA[63:0],m_axi_mm_video1_RRESP[1:0],m_axi_mm_video1_RLAST,m_axi_mm_video1_RVALID,m_axi_mm_video1_RREADY,m_axi_mm_video2_AWID[0:0],m_axi_mm_video2_AWADDR[31:0],m_axi_mm_video2_AWLEN[7:0],m_axi_mm_video2_AWSIZE[2:0],m_axi_mm_video2_AWBURST[1:0],m_axi_mm_video2_AWLOCK[1:0],m_axi_mm_video2_AWREGION[3:0],m_axi_mm_video2_AWCACHE[3:0],m_axi_mm_video2_AWPROT[2:0],m_axi_mm_video2_AWQOS[3:0],m_axi_mm_video2_AWVALID,m_axi_mm_video2_AWREADY,m_axi_mm_video2_WID[0:0],m_axi_mm_video2_WDATA[63:0],m_axi_mm_video2_WSTRB[7:0],m_axi_mm_video2_WLAST,m_axi_mm_video2_WVALID,m_axi_mm_video2_WREADY,m_axi_mm_video2_BID[0:0],m_axi_mm_video2_BRESP[1:0],m_axi_mm_video2_BVALID,m_axi_mm_video2_BREADY,m_axi_mm_video2_ARID[0:0],m_axi_mm_video2_ARADDR[31:0],m_axi_mm_video2_ARLEN[7:0],m_axi_mm_video2_ARSIZE[2:0],m_axi_mm_video2_ARBURST[1:0],m_axi_mm_video2_ARLOCK[1:0],m_axi_mm_video2_ARREGION[3:0],m_axi_mm_video2_ARCACHE[3:0],m_axi_mm_video2_ARPROT[2:0],m_axi_mm_video2_ARQOS[3:0],m_axi_mm_video2_ARVALID,m_axi_mm_video2_ARREADY,m_axi_mm_video2_RID[0:0],m_axi_mm_video2_RDATA[63:0],m_axi_mm_video2_RRESP[1:0],m_axi_mm_video2_RLAST,m_axi_mm_video2_RVALID,m_axi_mm_video2_RREADY,m_axi_mm_video3_AWID[0:0],m_axi_mm_video3_AWADDR[31:0],m_axi_mm_video3_AWLEN[7:0],m_axi_mm_video3_AWSIZE[2:0],m_axi_mm_video3_AWBURST[1:0],m_axi_mm_video3_AWLOCK[1:0],m_axi_mm_video3_AWREGION[3:0],m_axi_mm_video3_AWCACHE[3:0],m_axi_mm_video3_AWPROT[2:0],m_axi_mm_video3_AWQOS[3:0],m_axi_mm_video3_AWVALID,m_axi_mm_video3_AWREADY,m_axi_mm_video3_WID[0:0],m_axi_mm_video3_WDATA[63:0],m_axi_mm_video3_WSTRB[7:0],m_axi_mm_video3_WLAST,m_axi_mm_video3_WVALID,m_axi_mm_video3_WREADY,m_axi_mm_video3_BID[0:0],m_axi_mm_video3_BRESP[1:0],m_axi_mm_video3_BVALID,m_axi_mm_video3_BREADY,m_axi_mm_video3_ARID[0:0],m_axi_mm_video3_ARADDR[31:0],m_axi_mm_video3_ARLEN[7:0],m_axi_mm_video3_ARSIZE[2:0],m_axi_mm_video3_ARBURST[1:0],m_axi_mm_video3_ARLOCK[1:0],m_axi_mm_video3_ARREGION[3:0],m_axi_mm_video3_ARCACHE[3:0],m_axi_mm_video3_ARPROT[2:0],m_axi_mm_video3_ARQOS[3:0],m_axi_mm_video3_ARVALID,m_axi_mm_video3_ARREADY,m_axi_mm_video3_RID[0:0],m_axi_mm_video3_RDATA[63:0],m_axi_mm_video3_RRESP[1:0],m_axi_mm_video3_RLAST,m_axi_mm_video3_RVALID,m_axi_mm_video3_RREADY,s_axis_video_TVALID,s_axis_video_TREADY,s_axis_video_TDATA[23:0],s_axis_video_TKEEP[2:0],s_axis_video_TSTRB[2:0],s_axis_video_TUSER[0:0],s_axis_video_TLAST[0:0],s_axis_video_TID[0:0],s_axis_video_TDEST[0:0],m_axis_video_TVALID,m_axis_video_TREADY,m_axis_video_TDATA[23:0],m_axis_video_TKEEP[2:0],m_axis_video_TSTRB[2:0],m_axis_video_TUSER[0:0],m_axis_video_TLAST[0:0],m_axis_video_TID[0:0],m_axis_video_TDEST[0:0]";
attribute X_CORE_INFO : string;
attribute X_CORE_INFO of stub : architecture is "design_1_v_mix_0_0_v_mix,Vivado 2021.1_AR000033086_AR000033086_AR000033086";
begin
end;

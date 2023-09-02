import chisel3._
import chisel3.util.Cat

class LSFR extends Module {
  val io = IO(new Bundle {
    val start    = Input(UInt(1.W))
    val seed     = Input(UInt(8.W))
    val out      = Output(UInt(8.W))
    val seg1_out = Output(UInt(7.W))
    val seg2_out = Output(UInt(7.W))
  })

  val cnt            = RegInit(0.U(8.W))
  val start_reg      = RegInit(0.U(1.W))
  val start_postedge = Wire(UInt(1.W))
  val xor_bit        = Wire(UInt(1.W))

  start_postedge := io.start & ~start_reg
  xor_bit        := cnt(4) ^ cnt(3) ^ cnt(2) ^ cnt(0)
  io.out         := cnt

  when(start_postedge === 1.U) {
    cnt := io.seed
  }.elsewhen(start_reg === 1.U) {
    cnt := Cat(xor_bit, cnt(7, 1))
  }
  start_reg := io.start

  val seg1 = Module(new bcd7seg())
  seg1.io.en := io.start
  seg1.io.b  := io.out(7, 4)
  seg1.io.h  := io.seg1_out

  val seg2 = Module(new bcd7seg())
  seg2.io.en := io.start
  seg2.io.b  := io.out(3, 0)
  seg2.io.h  := io.seg2_out
}

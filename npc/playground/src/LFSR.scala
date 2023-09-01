import chisel3._
import chisel3.util.Cat

class LSFR extends Module {
  val io = IO(new Bundle {
    val start = Input(UInt(1.W))
    val seed  = Input(UInt(8.W))
    val out   = Output(UInt(8.W))
  })

  val cnt            = RegInit(0.U(8.W))
  val start_reg      = RegInit(0.U(1.W))
  val start_postedge = UInt(1.W)
  val xor_bit        = UInt(1.W)

  start_postedge := io.start & ~start_reg
  xor_bit        := cnt(4) ^ cnt(3) ^ cnt(2) ^ cnt(0)
  io.out         := cnt

  when(start_postedge === 1.U) {
    cnt := io.seed
  }.elsewhen(start_reg === 1.U) {
    cnt := Cat(xor_bit, cnt(7, 1))
  }
  start_reg := io.start
}

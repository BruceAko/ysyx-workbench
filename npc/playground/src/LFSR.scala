import chisel3._
import chisel3.util.Cat

class LSFR extends Module {
  val io = IO(new Bundle {
    val start = Input(Bool())
    val seed  = Input(UInt(8.W))
    val out   = Output(UInt(8.W))
  })

  val cnt       = RegInit(0.U(8.W))
  val start_reg = RegInit(false.B(Bool()))
  val xor_bit   = Wire(UInt(1.W))

  io.out := cnt

  start_reg := io.start

  when(io.start & ~start_reg === true.B) {
    cnt := io.seed
  }.elsewhen(start_reg === 1.U) {
    cnt := Cat(cnt(4) ^ cnt(3) ^ cnt(2) ^ cnt(0), cnt(7, 1))
  }
}

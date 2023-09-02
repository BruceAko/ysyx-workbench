import chisel3._
import chisel3.util.switch

class bcd7seg extends Module {
  val io = IO(new Bundle {
    val en = Input(UInt(1.W))
    val b  = Input(UInt(4.W))
    val h  = Output(UInt(7.W))
  })
  when(io.en === 1.U) {
    switch(io.b) {
      is(0.U) {
        io.h := 1.U
      }
    }
  }
}

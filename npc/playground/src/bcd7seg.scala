import chisel3._
import chisel3.util.switch
import chisel3.util.is

class bcd7seg extends Module {
  val io = IO(new Bundle {
    val en = Input(UInt(1.W))
    val b  = Input(UInt(4.W))
    val h  = Output(UInt(7.W))
  })
  io.h := "b0000000".U
  when(io.en === 1.U) {
    switch(io.b) {
      is(0.U) {
        io.h := "b0000001".U
      }
      is(1.U) {
        io.h := "b1001111".U
      }
      is(2.U) {
        io.h := "b0010010".U
      }
      is(3.U) {
        io.h := "b0000110".U
      }
      is(4.U) {
        io.h := "b1001100".U
      }
      is(5.U) {
        io.h := "b0100100".U
      }
      is(6.U) {
        io.h := "b0100000".U
      }
      is(7.U) {
        io.h := "b0001111".U
      }
      is(8.U) {
        io.h := "b0000000".U
      }
      is(9.U) {
        io.h := "b0000100".U
      }
      is(10.U) {
        io.h := "b0001000".U
      }
      is(11.U) {
        io.h := "b1100000".U
      }
      is(12.U) {
        io.h := "b0110001".U
      }
      is(13.U) {
        io.h := "b1000010".U
      }
      is(14.U) {
        io.h := "b0110000".U
      }
      is(15.U) {
        io.h := "b0111000".U
      }
    }
  }.otherwise {
    io.h := "b0000000".U
  }
}

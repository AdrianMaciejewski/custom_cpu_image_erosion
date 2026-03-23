import chisel3._
import chisel3.util._

class ProgramCounter extends Module {
  val io = IO(new Bundle {
    val stop = Input(Bool())
    val jump = Input(Bool())
    val run = Input(Bool())
    val programCounterJump = Input(UInt(16.W))
    val programCounter = Output(UInt(16.W))
  })

  val reg = RegInit(0.U(16.W))

  reg := Mux((!io.run)  || io.stop, reg, Mux(io.jump, io.programCounterJump, reg + 1.U))

  io.programCounter := reg

}

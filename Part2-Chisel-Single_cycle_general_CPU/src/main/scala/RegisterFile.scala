import chisel3._
import chisel3.util._

class RegisterFile extends Module {
  val io = IO(new Bundle {
    val aSel = Input(UInt(4.W))
    val bSel = Input(UInt(4.W))
    val writeEnable = Input(Bool())
    val writeData = Input(UInt(32.W))
    val writeSel = Input(UInt(4.W))
    val a = Output(UInt(32.W))
    val b = Output(UInt(32.W))
  })

  //val regs = Vec(16, RegInit(0.U(32.W)))
  val regs = RegInit(VecInit(Seq.fill(16)(0.U(32.W))))

  io.a := regs(io.aSel)
  io.b := regs(io.bSel)

  when(io.writeEnable) {
    regs(io.writeSel) := io.writeData
  }
}
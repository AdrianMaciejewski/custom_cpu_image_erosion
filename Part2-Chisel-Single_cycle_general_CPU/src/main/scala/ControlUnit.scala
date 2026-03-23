import chisel3._
import chisel3.util._

class ControlUnit extends Module {
  val io = IO(new Bundle {
    val opcode = Input(UInt(4.W))

    // Outputs
    val stop = Output(Bool())
    val aluOp = Output(AluOp())
    val aluUsingImm = Output(Bool())
    val branch = Output(Bool())
    val dataReadEnable = Output(Bool())
    val dataWriteEnable = Output(Bool())
    val registerWrite = Output(Bool())

  })

  // default

  io.aluOp           := AluOp.add
  io.aluUsingImm     := false.B
  io.stop            := false.B
  io.branch          := false.B
  io.dataReadEnable  := false.B
  io.dataWriteEnable := false.B
  io.registerWrite   := false.B

  switch(io.opcode) {
    is(1.U)  { io.aluOp := AluOp.add; io.registerWrite := true.B }       // add
    is(2.U)  { io.aluOp := AluOp.sub; io.registerWrite := true.B }       // sub
    is(3.U)  { io.aluOp := AluOp.add; io.aluUsingImm := true.B; io.registerWrite := true.B } // addi
    is(4.U)  { io.aluOp := AluOp.mul; io.aluUsingImm := true.B; io.registerWrite := true.B } // muli
    is(5.U)  { io.dataWriteEnable := true.B }                            // store
    is(6.U)  { io.dataReadEnable := true.B; io.registerWrite := true.B } // load
    is(9.U)  { io.aluOp := AluOp.eql; io.branch := true.B }              // beq
    is(10.U) { io.aluOp := AluOp.neq; io.branch := true.B }              // bne
    is(11.U) { io.aluOp := AluOp.geq; io.branch := true.B }              // bge
    is(15.U) { io.stop := true.B }                                       // exit
  }

}
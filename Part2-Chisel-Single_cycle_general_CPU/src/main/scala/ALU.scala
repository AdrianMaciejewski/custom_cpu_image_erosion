import chisel3._
import chisel3.util._

object AluOp extends ChiselEnum {
  val add, mul, sub, neq, eql, geq = Value // bit width is determined automatically based on the number of values
}

class ALU extends Module {
  val io = IO(new Bundle {
    val val1: UInt = Input(UInt(32.W))
    val val2: UInt = Input(UInt(32.W))
    val opCode: AluOp.Type = Input(AluOp())
    val result: UInt = Output(UInt(32.W))
    val isOverflow: Bool = Output(Bool())
    val isBooleanOpTrue: Bool = Output(Bool())
  })

  io.result := 0.U
  io.isOverflow := false.B
  io.isBooleanOpTrue := false.B

  switch(io.opCode) {
    is(AluOp.add) {
      // 33-bit add to get the carry-out for unsigned overflow
      val sum = io.val1 +& io.val2           // UInt(33.W)
      io.result := sum(31, 0)
      io.isOverflow := sum(32)               // carry-out bit
    }

    is(AluOp.mul) {
      // 64-bit product; overflow if upper 32 bits are non-zero
      val prod = io.val1 * io.val2
      io.result := prod(31, 0)
      io.isOverflow := prod(63, 32) =/= 0.U
    }

    is(AluOp.sub) {
      val diff = io.val1 - io.val2
      io.result := diff
      io.isOverflow := io.val1 < io.val2
    }

    is(AluOp.neq) {
      val neqTrue = io.val1 =/= io.val2
      io.isBooleanOpTrue := neqTrue
      io.result := Mux(neqTrue, 1.U, 0.U)
    }

    is(AluOp.eql) {
      val eqTrue = io.val1 === io.val2
      io.isBooleanOpTrue := eqTrue
      io.result := Mux(eqTrue, 1.U, 0.U)
    }

    is(AluOp.geq) {
      val geqTrue = io.val1 >= io.val2
      io.isBooleanOpTrue := geqTrue
      io.result := Mux(geqTrue, 1.U, 0.U)
    }
  }

}
/* ==== LineDraw ==== */

/* WARNING: Unknown calling convention */

void LineDraw(MeReal *start,MeReal *end,MeReal r,MeReal g,MeReal b)

{
  if (doneError != 0) {
    return;
  }
  MeInfo(0,"Cannot draw debug line: No Debug Line Drawing Function Set.");
  return;
}



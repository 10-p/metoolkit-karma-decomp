/* empty.c — the entire content of a metoolkit_decomp_dummy archive.
 *
 * This is not an oversight and it is not a placeholder waiting to be filled in.
 * MathEngine's own metoolkit_dummy is exactly this: sixteen 680-byte archives,
 * byte-identical to one another, each holding one object compiled from
 * `C:\Projects\tmp\Dummy\D1.c` that defines no symbols. Their job is to satisfy
 * a link line that always names the Karma libraries, in a build where the calls
 * to Karma have been compiled away.
 *
 * If you are here because the linker cannot find McdInit or MdtWorldCreate:
 * that is this file working. Compile the Karma calls out as well.
 *
 * The translation unit needs one external definition to be strictly conforming;
 * this one is deliberately named so it cannot collide with anything in Karma.
 */
const char metoolkit_decomp_dummy_marker[] =
    "metoolkit_decomp_dummy: no physics is linked into this binary";

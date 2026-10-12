int minor_pf(void);

/*
 * Called outside the timed region of each pass. The defaults do nothing; a
 * client may define its own, e.g. to count kernel entries over the pass.
 */
void minor_pf_pass_begin(void);
void minor_pf_pass_end(const char *name);

/* Errors */
SELECT 'notexists.pdf'::pdf;
SELECT '/tmp/bad.pdf'::pdf;

/* OK */
SELECT '/tmp/pgintro.pdf'::pdf;

/* API */
SELECT pdf_title('/tmp/pgintro.pdf');
SELECT pdf_author('/tmp/pgintro.pdf');
SELECT pdf_num_pages('/tmp/pgintro.pdf');
SELECT pdf_page('/tmp/pgintro.pdf', 1);

/* bytea -> pdf */
SELECT pg_read_binary_file('/tmp/pgintro.pdf')::pdf::text = '/tmp/pgintro.pdf'::pdf::text;

/* pdf -> bytea */
SELECT '/tmp/pgintro.pdf'::pdf::bytea = pg_read_binary_file('/tmp/pgintro.pdf');

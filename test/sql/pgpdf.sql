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

/* FTS */
SELECT '/tmp/pgintro.pdf'::pdf::text @@ to_tsquery('postgres');
SELECT '/tmp/pgintro.pdf'::pdf::text @@ to_tsquery('oracle');

/* Old functions mentioned in the blog post. Making sure they continue to work */

SELECT pdf_read_file('/tmp/pgintro.pdf') = '/tmp/pgintro.pdf'::pdf::text;
select pdf_read_bytes(pg_read_binary_file('/tmp/pgintro.pdf')) = '/tmp/pgintro.pdf'::pdf::text;
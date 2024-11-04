select pdf_read_file('/tmp/pgintro.pdf');

select pdf_read_bytes(pg_read_binary_file('/tmp/pgintro.pdf'));

select '/tmp/aabbcc.pdf'::pdf;
select '/tmp/bad.pdf'::pdf;
select '/tmp/pgintro.pdf'::pdf;
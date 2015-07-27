function hdr = readpvpheader(fid,pos)
% hdr = readpvpheader(fid,pos)
% fid is an open file id.  The position in the stream can be anything
% on entry; on exit it is at the end of the header.
% pos gives the file position.  If absent, use the current position.
% If a nonnegative integer, use that position, measured forward from the end
% of the file.
%
% It is an error to use a negative position or a position large enough that
% there isn't room for a header between the position and the end of the
% file, although the routine is too lazy to check.
%
% hdr is a struct whose fields are:
%     headersize
%     numparams
%     filetype
%     nx
%     ny
%     nf
%     numrecords
%     recordsize
%     datasize
%     datatype
%     nxprocs
%     nyprocs
%     nxGlobal
%     nyGlobal
%     kx0
%     ky0
%     nb
%     nbands
%     time
%
% If hdr.numparams is bigger than 20, there is a field 'additional'
% containing an vector of hdr.numparams-20 elements.

if exist('pos','var')
    fseek(fid,pos,'bof');
end

headerwords = fread(fid,18,'int32');
hdr.headersize = headerwords(1);
hdr.numparams = headerwords(2);
hdr.filetype = headerwords(3);
hdr.nx = headerwords(4);
hdr.ny = headerwords(5);
hdr.nf = headerwords(6);
hdr.numrecords = headerwords(7);
hdr.recordsize = headerwords(8);
hdr.datasize = headerwords(9);
hdr.datatype = headerwords(10);
hdr.nxprocs = headerwords(11);
hdr.nyprocs = headerwords(12);
hdr.nxGlobal = headerwords(13);
hdr.nyGlobal = headerwords(14);
hdr.kx0 = headerwords(15);
hdr.ky0 = headerwords(16);
hdr.nbatch = headerwords(17);
hdr.nbands = headerwords(18);
hdr.time = fread(fid,1,'float64');

if hdr.numparams>20
    hdr.additional = fread(fid,hdr.numparams-20,'int32');
end

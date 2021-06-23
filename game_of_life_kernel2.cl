kernel void rot13(global char* in, global char* out)
{
  int num = get_global_id(0);
  char c = in[num];
  if ('a' <= c && c <= 'z') {
    out[num] = ((c - 'a') + 13) % 26 + 'a';
  } else if ('A' <= c && c <= 'Z') {
    out[num] = ((c - 'A') + 13) % 26 + 'A';
  } else {
    out[num] = c;
  }
}
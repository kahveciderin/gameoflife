use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::path::Path;
use std::io::prelude::*;

fn pause() {
    let mut stdin = io::stdin();
    let mut stdout = io::stdout();

    // We want the cursor to stay at the end of the line, so we print without a newline and flush manually.
    write!(stdout, "\nPress any key to continue...").unwrap();
    stdout.flush().unwrap();

    // Read a single byte and discard
    let _ = stdin.read(&mut [0u8]).unwrap();
}

fn gcel(_x: i8, _y: i8, pf: [bool; 256]) -> bool{
    let mut x: i8 = _x;
    let mut y: i8 = _y;
    if x < 0{
        x = 16 + x;
    }
    if x > 15{
        x = x % 16;
    }
    if y < 0{
        y = 16 + y
    }
    if y > 15{
        y = y % 16
    }
    return pf[((16 as i32 * y as i32)as i32 as i32 + x as i32) as i32 as usize];
}

fn main() {

    let checks: [[i8; 2]; 8] = [[-1, -1], [0, -1], [1, -1], [-1, 0], [1, 0], [-1, 1], [0, 1], [1, 1]];
    let mut playfield: [bool; 256] = [false; 256]; // wanted to use 0 but it didn't allow me
    
    let filename = "test.rle";
    // Open the file in read-only mode (ignoring errors).
    let file = File::open(filename).unwrap();
    let reader = BufReader::new(file);
    let mut lineno: i8 = 0;
    let mut data = String::from("");

    let mut surv: [i32; 8] = [2, 3, 9, 9, 9, 9, 9, 9];
    let mut born: [i32; 8] = [3, 9, 9, 9, 9, 9, 9, 9];


    let mut patxsize: i32 = 0;
    let mut patysize: i32 = 0;

    let mut fef: bool = false;

    // Read the file line by line using the lines() iterator from std::io::BufRead.
    for (index, line) in reader.lines().enumerate() {
        let line = line.unwrap(); // Ignore errors.
        let a = line;
        let char_vec: Vec<char> = a.chars().collect();
        if char_vec[0] == '#'{
            continue
        }

        if lineno == 0{ // first line
            let mut x_dim: [i32; 4] = [0, 0, 0, 0];
            let mut khj: i32 = 4;
            while (char_vec.len() > (khj as usize)) && char_vec[khj as usize].is_digit(10){
                x_dim[khj as usize - 4] = char_vec[khj as usize] as i32 - 0x30;
                khj += 1;
            }
            patxsize = ((x_dim[0] as i32 * 1) + (x_dim[1] as i32 * 10) + (x_dim[2] as i32 * 100) + (x_dim[3] as i32 * 1000)).into();

            khj += 6;
            let mut afs: i32 = 0;
            let mut y_dim: [i32; 4] = [0, 0, 0, 0];
            while (char_vec.len() > (khj as usize)) && char_vec[khj as usize].is_digit(10){
                y_dim[afs as usize] = char_vec[khj as usize] as i32 - 0x30;
                khj += 1;
                afs += 1;
            }
            khj += 10;

            patysize = ((y_dim[0] as i32 * 1) + (y_dim[1] as i32 * 10) + (y_dim[2] as i32 * 100) + (y_dim[3] as i32 * 1000)).into();

            afs = 0;
            while (char_vec.len() > (khj as usize)) && char_vec[khj as usize].is_digit(10){
                born[afs as usize] = char_vec[khj as usize] as i32 - 0x30;
                khj += 1;
                afs += 1;
            }
            
            khj += 2;
            afs = 0;


            while (char_vec.len() > (khj as usize)) && char_vec[khj as usize].is_digit(10){
                surv[afs as usize] = char_vec[khj as usize] as i32 - 0x30;
                khj += 1;
                afs += 1;
                fef = true;
            }


        }else{
            data.push_str(&a[..]);
        }
        lineno += 1;
    }

    let offset = (((16 / 2) - (patysize / 2)) * 16) + ((16 / 2) - (patxsize / 2));

    let mut tmpx: i32 = 0;
    let mut tmpy: i32 = 0;
    let mut digit: [i32; 4] = [1, 0, 0, 0];
    let mut did: i8 = -1;
    let mut ad: i32 = 0;
    for c in data.chars(){
        if c == '$'{
            tmpy += 1;
            tmpx = 0;
            digit = [1, 0, 0, 0];
            did = -1;
            ad = tmpy * 16;
        }else if c == '!'{
            break
        }else if c.is_digit(10){
            did += 1;
            digit[did as usize] = (c as i32) - 0x30;
        }else if c == 'b'{
            let dval: i32 = ((digit[0] as i32 * 1) + (digit[1] as i32 * 10) + (digit[2] as i32 * 100) + (digit[3] as i32 * 1000)).into();
            tmpx += dval;
            for i in 0..dval{
                playfield[offset as usize + ((i + ad) )as usize] = false;
            }
            digit = [1, 0, 0, 0];
            did = -1;
            ad += dval;
        }else if c == 'o'{
            let dval: i32 = ((digit[0] * 1) + (digit[1] * 10) + (digit[2] * 100) + (digit[3] * 1000)).into();
            tmpx += dval;
            for i in 0..dval{
                playfield[offset as usize + ((i + ad) )as usize] = true;
            }
            digit = [1, 0, 0, 0];
            did = -1;
            ad += dval;
        }
    }

    loop {
        print!("\x1B[2J\x1B[1;1H"); // clear the screen 
        let mut printstr = String::from("");
        let mut newbuf: [bool; 256] = [false; 256];
        for i in 0..playfield.len() {
            if i % 16 == 0{
                printstr.push('\n') // - are semicolons optional when scope is about to terminate?
            }
            if playfield[i] {
                printstr.push('*')
            } 
            else {
                printstr.push(' ')
            }
            let mut neigh: i32 = 0;
            for check in checks{
                if gcel(((i % 16) as i8 + check[0]), ((i / 16) as i8 + check[1] ), playfield){
                    neigh += 1;
                }
            }
            newbuf[i] = false;

            if neigh == born[0] || neigh == born[1] || neigh == born[2] || neigh == born[3] || neigh == born[4] || neigh == born[5] || neigh == born[6] || neigh == born[7]{
                newbuf[i] = true
            }else if (neigh == surv[0] || neigh == surv[1] || neigh == surv[2] || neigh == surv[3] || neigh == surv[4] || neigh == surv[5] || neigh == surv[6] || neigh == surv[7]) && playfield[i]{
                newbuf[i] = true
            }
        }
        playfield = newbuf;
        print!("{ps}", ps=printstr); // printing done
        pause()
    }
}
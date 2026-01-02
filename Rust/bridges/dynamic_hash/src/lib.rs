/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */
mod eval;
mod interop;
mod parse;

#[cfg(test)]
mod tests;

#[derive(Copy, Clone, Default, PartialEq, Eq)]
pub enum OutputFormat {
    #[default]
    Default,
    Hex,
    Binary,
    Base64,
}

#[derive(Copy, Clone, Default, PartialEq, Eq)]
pub enum DataDecoder {
    #[default]
    None,
    Unhex,
    B64Decode,
}

pub enum ExtraParams {
    Key(Box<Expr>),
    StartLength(u32, u32),
    CostSalt(Box<Expr>, Box<Expr>),
    RoundsSaltDklen(Box<Expr>, Box<Expr>, Box<Expr>),
}

pub enum Expr {
    Call {
        name: String,
        arg: Box<Expr>,
        params: Option<ExtraParams>,
        output_format: OutputFormat,
    },
    Concat(Vec<Expr>),
    Var((String, DataDecoder)),
    Literal(Vec<u8>),
    Number(u32),
}

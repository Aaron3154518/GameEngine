import { parse, v4 } from 'uuid';

// Random UUID
export type UUID = Uint8Array;

export function newUUID(): string {
  return v4();
}

// Access attribute by string
export function getAttr(obj: any, attrs: string): any {
  return attrs.split('.').reduce((obj: any, attr: string) => obj[attr], obj);
}

// Search filtering
export function searchScore<T>(
  search: string,
  getSearchStrings: (t: T) => string[]
): (item: T) => number {
  return (item: T) =>
    getSearchStrings(item).reduce((s: number, str: string) => {
      let idx: number = str.indexOf(search);
      return s + (idx === -1 ? 0 : idx === 0 ? 1 : 0.5);
    }, 0);
}

export function sortList<T>(
  list: T[],
  scoreFunc: (a: T) => number,
  ascending: boolean = true
) {
  list.sort(
    (a: T, b: T) => (scoreFunc(b) - scoreFunc(a)) * (ascending ? 1 : -1)
  );
}

// Variable/type regexes
export const alpha_: string = 'a-zA-Z_';
export const alphanum_: string = `${alpha_}0-9`;
export const varRegex: string = `[${alpha_}][${alphanum_}]*`;
export const typeRegex: string = `(${varRegex})(::(${varRegex})+)*`;
export const typeVarRegex: string = `${typeRegex} ${varRegex}`;

export function validateVar(s: string): boolean {
  return RegExp(`^${varRegex}$`).test(s);
}

// export function sanitizeVar(s: string, i: number): string {
//   let regExp: string = `[${alpha_}${i === 0 ? '' : '0-9'}]`;

//   return RegExp(`${regExp}`).test(s.charAt(i))
//     ? s
//     : s.substring(0, i) + s.substring(i + 1);
// }

export function validateType(s: string): boolean {
  return RegExp(`^${typeRegex}$`).test(s);
}

// export function sanitizeType(s: string, i: number): string {
//   let numPreColon: number =
//     s.charAt(i - 1) === ':' ? (s.charAt(i - 2) === ':' ? 2 : 1) : 0;
//   let numPostColon: number =
//     s.charAt(i + 1) === ':' ? (s.charAt(i + 2) === ':' ? 2 : 1) : 0;
//   let regExp: string = Object.entries({
//     'a-zA-Z_': i === 0 || (numPreColon !== 1 && numPostColon !== 1),
//     '0-9': i !== 0 && numPreColon === 0 && numPostColon !== 1,
//     ':': i !== 0 && numPreColon + numPostColon !== 2,
//   }).reduce((s: string, [k, v]: [string, boolean]) => (v ? s + k : s), '');

//   return !regExp || RegExp(`[^${regExp}]`).test(s.charAt(i))
//     ? s.substring(0, i) + s.substring(i + 1)
//     : s;
// }

export function validateTypeVar(s: string): boolean {
  return RegExp(`^${typeVarRegex}$`).test(s);
}

// export function sanitizeTypeVar(s: string, i: number): string {
//   let split: string[] = s.split(' ');
//   if (split.length === 1) {
//     return sanitizeType(s, i);
//   }
//   if (split.length > 2 && s.charAt(i) === ' ') {
//     return s.substring(0, i) + s.substring(i + 1);
//   }
//   // Split length is now 2
//   if (i < split[0].length) {
//     // i is in the type
//     return `${sanitizeType(split[0], i)} ${split[1]}`;
//   }
//   if (i > split[0].length) {
//     // i is in the var
//     return `${split[0]} ${sanitizeVar(split[1], i - split[0].length - 1)}`;
//   }
//   // i is the first space
//   if (!split[0] || !validateType(split[0])) {
//     return s.substring(0, i) + s.substring(i + 1);
//   }
//   return s;

//   // let split: string[] = s.split(' ');
//   // let isVar: boolean = split.length > 1;
//   // return `${sanitizeType(split[0])}${
//   //   isVar && validateType(split[0]) ? ' ' : ''
//   // }${isVar ? sanitizeVar(split.slice(1).join('')) : ''}`;
// }

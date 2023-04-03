import {
  AfterViewInit,
  Component,
  ElementRef,
  Input,
  OnChanges,
  OnInit,
  QueryList,
  SimpleChanges,
  ViewChild,
  ViewChildren,
} from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import {
  Callback,
  CallbackParameter,
  CodeType,
  Parameters,
  StringDict,
} from '../utils/interfaces';
import { alphanum_, validateVar } from '../utils/utils';
import { InputComponent } from '../search/input/input.component';
import { CallbackParameterList } from '../utils/interfaces';

@Component({
  selector: 'app-parameter',
  templateUrl: './parameter.component.html',
  styleUrls: ['./parameter.component.css'],
})
export class ParameterComponent implements OnInit, AfterViewInit, OnChanges {
  @ViewChild('codeDiv', { static: true })
  codeDiv?: ElementRef<HTMLDivElement>;
  @ViewChild('codeDisplay', { static: true })
  codeDisplay?: ElementRef<HTMLDivElement>;
  @ViewChild('codeInput', { static: true })
  codeInput?: ElementRef<HTMLTextAreaElement>;
  @ViewChild('lineNums', { static: true })
  lineNums?: ElementRef<HTMLDivElement>;
  @ViewChildren('codeLine') codeLines?: QueryList<ElementRef<HTMLDivElement>>;
  @ViewChildren('paramInput') paramInputs?: QueryList<InputComponent>;

  @Input() callback: Callback = new Callback();
  parameters: CallbackParameterList[] = [];
  signature: [string, CodeType][] = [];
  code: string = '';
  name: string = '';
  idxs: number[][] = [[0, 0]];
  x_offs: number[][] = [];

  CodeType = CodeType;
  validateVar = validateVar;

  constructor(protected parameterService: ParameterService) {}

  ngOnInit() {}

  ngAfterViewInit(): void {
    this.codeLines?.changes.subscribe(() => {
      this.updateText();
    });
    this.updateText();
  }

  ngOnChanges(changes: SimpleChanges) {
    if (changes['callback']) {
      this.name = this.callback.name;
      if (!this.callback.code.endsWith('\n')) {
        this.callback.code += '\n';
      }
      this.code = this.callback.code;
      this.callback.$changes.subscribe(() => {
        this.parameters = this.callback.getParameters(this.parameterService);
      });
      this.parameters = this.callback.getParameters(this.parameterService);

      setTimeout(() => {
        if (this.codeInput && this.codeDisplay && this.codeDiv) {
          this.codeInput.nativeElement.style.height = `${this.codeDisplay.nativeElement.offsetHeight}px`;
          this.codeInput.nativeElement.scrollTo(0, 0);
          this.codeDiv.nativeElement.scrollTo(0, 0);
        }
      }, 0);
    }
  }

  update() {
    if (!this.name || !this.code) {
      return;
    }

    this.callback.code = this.code;
  }

  updateText() {
    if (this.codeLines) {
      let x_offs: number[][] = [...Array(this.codeLines.length)].map(() => []);
      this.codeLines.forEach((el: ElementRef<HTMLDivElement>) => {
        let div: HTMLDivElement = el.nativeElement;
        let id: number = +div.id;
        let x: number = 0;
        Object.values(div.children).forEach((child: Element) => {
          x_offs[id].push(x);
          x += child.clientWidth;
        });
      });
      if (
        x_offs.findIndex((row: number[], r: number) => {
          return (
            this.x_offs[r] === undefined ||
            row.findIndex((v: Number, c: number) => v !== this.x_offs[r][c]) !==
              -1
          );
        }) !== -1
      ) {
        setTimeout(() => {
          this.x_offs = x_offs;
        }, 0);
      }
    }
  }

  onCodeChanged(event: Event) {
    this.code = (event.target as HTMLTextAreaElement).value;
  }

  onNameChanged(event: Event) {
    this.name = (event.target as HTMLInputElement).value;
  }

  onScroll(event: Event) {
    let scrollX: number = (event.target as HTMLTextAreaElement).scrollLeft;
    let scrollY: number = (event.target as HTMLTextAreaElement).scrollTop;
    if (this.codeDisplay) {
      this.codeDisplay.nativeElement.scrollTo(scrollX, scrollY);
    }
    if (this.lineNums) {
      this.lineNums.nativeElement.scrollTo(scrollX, scrollY);
    }
  }

  onCodeKeyPress(event: KeyboardEvent) {
    if (event.ctrlKey && event.key === 's') {
      this.save();
      event.preventDefault();
    }
  }

  onParamEnter(param: CallbackParameter, ic: InputComponent) {
    this.code = this.code.replace(param.alias, ic.value);
    param.alias = ic.value;
  }

  save() {
    if (this.name) {
      this.callback.name = this.name;
    }
    if (!this.code.endsWith('\n')) {
      this.code += '\n';
    }
    this.callback.code = this.code;
    this.callback.setParameterAliases(this.parameters);
  }

  select(uuid: string, name: string) {
    console.log('Show Parameters:', uuid, name);
  }

  isNameUsed(): boolean {
    return false;
  }

  getVarIdxs(): number[][] {
    let regex: string =
      `(^|[^${alphanum_}])(` +
      Object.values(this.callback.params)
        .map((params: StringDict<string>) =>
          Object.entries(params)
            .map(([p, a]: [string, string]) => (a ? a : p))
            .join('|')
        )
        .join('|') +
      `)([^${alphanum_}]|$)`;
    let global_idx: number = 0;
    return this.code.split('\n').map((l: string) => {
      if (!l) {
        global_idx += 1;
        return [];
      }
      let res: RegExpMatchArray | null = l.match(regex);
      let idxs: number[] = [global_idx];
      while (res && res.index !== undefined) {
        let idx: number = res.index + res[1].length;
        idxs.push(global_idx + idx);
        idx += res[2].length;
        idxs.push(global_idx + idx);
        global_idx += idx;
        l = l.slice(idx);
        res = l.match(regex);
      }
      global_idx += l.length;
      idxs.push(global_idx);
      global_idx += 1;
      return idxs;
    });
  }
}

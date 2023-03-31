import { Component, Input, OnInit } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import {
  UUID,
  sanitizeType,
  sanitizeVar,
  searchScore,
  sortList,
  validateType,
  validateVar,
} from '../utils/utils';
import { ParameterGroup, Parameters, StringDict } from '../utils/interfaces';
import {
  ColComponent,
  ColHeaderComponent,
  Column,
  ColWidth,
} from '../search/search.component';
import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'parametergroup',
  pure: false,
})
export class ParameterGroupPipe implements PipeTransform {
  transform(
    items: Set<UUID>,
    parameterService: ParameterService
  ): ParameterGroup[] {
    return [...items]
      .map((id: UUID) => parameterService.getParamGroup(id))
      .filter((g: ParameterGroup) => g.params.size > 0);
  }
}

@Component({
  selector: 'type-col-header',
  templateUrl: '../search/col-header.component.html',
})
export class TypeColHeaderComponent extends ColHeaderComponent {
  constructor() {
    super();
    this.classes = this.classes.concat([
      'type',
      'border-end-0',
      'rounded-0',
      'rounded-start',
      'text-end',
    ]);
  }
}

@Component({
  selector: 'type-col-header',
  templateUrl: '../search/col-header.component.html',
})
export class NameColHeaderComponent extends ColHeaderComponent {
  constructor() {
    super();
    this.classes = this.classes.concat([
      'border-start-0',
      'rounded-0',
      'rounded-end',
    ]);
  }
}

@Component({
  selector: 'app-group',
  template: `
    <ng-container
      *ngFor="
        let group of value | parametergroup : parameterService;
        let first = first
      "
      ><span *ngIf="!first" class="vertical-line h-100"></span
      ><span
        class="hover"
        [ngClass]="[first ? 'ms-2 me-1' : 'mx-1']"
        [title]="group.name"
        ><app-var [value]="group.params"></app-var></span
    ></ng-container>
  `,
  styles: [
    `
      .hover:hover {
        background-color: rgba(200, 200, 200, 0.25);
      }
    `,
  ],
})
export class GroupComponent implements ColComponent {
  @Input() value: Set<UUID> = new Set<UUID>();

  constructor(protected parameterService: ParameterService) {}
}

// TODO: delete type-modal
@Component({
  selector: 'app-parameters-search',
  templateUrl: './parameters-search.component.html',
  styleUrls: ['./parameters-search.component.css'],
})
export class ParametersSearchComponent {
  cols: Column[] = [
    new Column({
      key: 'type',
      width: ColWidth.Fit,
      requireInput: true,
      inputPlaceholder: '+ New Type',
      validateInput: validateType,
      sanitizeInput: sanitizeType,
      component: TypeColHeaderComponent,
    }),
    new Column({
      key: 'name',
      width: ColWidth.Fit,
      requireInput: true,
      inputPlaceholder: 'Name',
      validateInput: (s: string): boolean =>
        validateVar(s) &&
        this.parameterService.paramSets.findIndex(
          (set: Parameters) => set.name === s
        ) === -1,
      sanitizeInput: sanitizeVar,
      component: NameColHeaderComponent,
    }),
    new Column({
      key: 'groups',
      component: GroupComponent,
    }),
  ];

  constructor(protected parameterService: ParameterService) {}

  sortParamSets(rows: Parameters[], query: string) {
    sortList(
      rows,
      searchScore(query, (set: Parameters) => [set.name, set.type])
    );
  }

  newSet(args: StringDict<string>) {
    this.parameterService.newParamSet(
      new Parameters({
        type: args['type'],
        name: args['name'],
      })
    );
  }
}
